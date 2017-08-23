//
// Created by zhangkuo on 17-8-6.
//
#include "EventLoop.h"
#include <sys/eventfd.h>
#include "Poller.h"
#include "SocketOps.h"

using namespace WebServer;

namespace
{
    __thread EventLoop* t_loopInThisThread = 0;

    int createEventFd()
    {
        int evtFd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        return evtFd;
    }

    const int kPollTimeMs = 10000;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      poller_(Poller::newDefaultPoller(this)),
      wakeupFd_(createEventFd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      threadId_(CurrentThread::tid())

{
    t_loopInThisThread = this;

    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
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
        MutexLock lock(mutex_);
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
        MutexLock lock(mutex_);
        pendingFunctors_.push_back(std::move(cb));
    }

    if (!isInLoopThread() || callingPendingFunctors_)
    {
        cb();
    }
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

bool EventLoop::hasChannel()
{

}


void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    poller_->removeChannel(channel);
}

void EventLoop::doPendingFunctors()
{

}

