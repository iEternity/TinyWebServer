//
// Created by zhangkuo on 17-8-6.
//
#include <sys/eventfd.h>
#include <signal.h>
#include <assert.h>
#include <xnet/net/EventLoop.h>
#include <xnet/net/SocketOps.h>
#include <xnet/net/TimerQueue.h>
#include <xnet/net/Poller.h>
#include <xnet/base/Logging.h>

using namespace xnet;

namespace
{
    thread_local EventLoop* t_loopInThisThread = 0;
    const int kPollTimeMs = 10000;

    int createEventFd()
    {
        int eventFd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if(eventFd < 0)
        {
            LOG_SYSFATAL << "Failed to create eventfd";
        }
        return eventFd;
    }

    /*
     * 在linux上编写socket程序时，如果向一个一关闭的套接字发送消息时，第一次发送对端会回应RST报文，
     * 第二次发送则系统会抛出SIGPIPE信号，默认该信号的处理方式是终止进程,这里直接忽略该信号
     */
    class IgnoreSigPipe
    {
    public:
        IgnoreSigPipe()
        {
            ::signal(SIGPIPE, SIG_IGN);
        }
    };

    IgnoreSigPipe initObj;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      eventHandling_(false),
      callingPendingFunctors_(false),
      iteration_(0),
      threadId_(CurrentThread::tid()),
      poller_(Poller::newDefaultPoller(this)),
      timerQueue_(new TimerQueue(this)),
      wakeupFd_(createEventFd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      currentActiveChannel_(NULL)

{
    LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
    if(t_loopInThisThread)
    {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                  << " exists in this thread " << threadId_;
    }
    else
    {
        t_loopInThisThread = this;
    }

    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_
              << " destructs in thread " << CurrentThread::tid();

    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();

    looping_ = true;
    quit_ = false;

    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        iteration_++;

        eventHandling_ = true;

        for(auto& channel:activeChannels_)
        {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }

        currentActiveChannel_ = NULL;
        eventHandling_ = false;
        doPendingFunctors();
    }

    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;

    if (!isInLoopThread())
    {
        wakeup();
    }

}

void EventLoop::runInLoop(const Functor& cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}

void EventLoop::runInLoop(Functor&& cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(const Functor& cb)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }

    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

void EventLoop::queueInLoop(Functor&& cb)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(std::move(cb));
    }

    if (!isInLoopThread() || callingPendingFunctors_)
    {
        cb();
    }
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
    timerQueue_->addTimer(cb, time, 0);
}

TimerId EventLoop::runAt(const Timestamp& time, TimerCallback&& cb)
{
    return timerQueue_->addTimer(std::move(cb), time, 0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runAfter(double delay, TimerCallback&& cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

TimerId EventLoop::runEvery(double interval, TimerCallback&& cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(std::move(cb), time, interval);
}

void EventLoop::cancel(TimerId timerId)
{
    timerQueue_->cancel(timerId);
}

void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    return poller_->hasChannel(channel);
}

void EventLoop::wakeup()
{
    uint64_t n = 1;
    sockets::write(wakeupFd_, &n, sizeof n);
}

void EventLoop::handleRead()
{
    uint64_t n;
    sockets::read(wakeupFd_, &n, sizeof n);
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(int i=0; i<functors.size(); i++)
    {
        functors[i]();
    }

    callingPendingFunctors_ = false;
}

void EventLoop::assertInLoopThread() const
{
    if(!isInLoopThread())
    {
        abortNotInLoopThread();
    }
}

void EventLoop::abortNotInLoopThread() const
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}