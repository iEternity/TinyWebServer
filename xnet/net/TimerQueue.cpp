//
// Created by zhangkuo on 17-8-25.
//
#include <sys/timerfd.h>
#include <string.h>
#include <xnet/base/Logging.h>
#include <xnet/net/TimerQueue.h>

using namespace xnet;

TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop),
      timerfd_(createTimerfd()),
      timerfdChannel_(loop, timerfd_),
      timers_(),
      callingExpiredTimers_(false)
{
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);
}

int TimerQueue::createTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0)
    {
        LOG_SYSFATAL << "Failed to create timerfd";
    }

    return timerfd;
}

void TimerQueue::readTimerfd() const
{
    uint64_t howMany;
    ssize_t n = ::read(timerfd_, &howMany, sizeof(howMany));
    if(n != sizeof(howMany))
    {
        LOG_ERROR << "TimerQueue::readTimerfd() reads " << n << " bytes instead of 8 bytes";
    }
}

void TimerQueue::resetTimerfd(Timestamp expiration)
{
    struct itimerspec oldVal;
    struct itimerspec newVal;
    ::bzero(&oldVal, sizeof(oldVal));
    ::bzero(&newVal, sizeof(newVal));

    auto howMuchTimeFromNow = [](Timestamp expiration) ->timespec
    {
        int64_t microseconds = expiration.microsecondsSinceEpoch() - Timestamp::now().microsecondsSinceEpoch();
        if(microseconds < 100)
        {
            microseconds = 100;
        }

        struct timespec ts;
        ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicrosecondsPerSecond);
        ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicrosecondsPerSecond) * 1000);

        return ts;
    };

    newVal.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd_, 0, &newVal, &oldVal);
    if(ret < 0)
    {
        LOG_SYSERR << "timerfd_settime()";
    }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
    TimerPtr timer = std::make_shared<Timer>(cb, when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

TimerId TimerQueue::addTimer(TimerCallback&& cb, Timestamp when, double interval)
{
    TimerPtr timer = std::make_shared<Timer>(std::move(cb), when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::addTimerInLoop(TimerPtr timer)
{
    bool earliestChanged = insert(timer);

    if(earliestChanged)
    {
        resetTimerfd(timer->expiration());
    }
}

bool TimerQueue::insert(TimerPtr timer)
{
    loop_->assertInLoopThread();

    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    auto it = timers_.begin();
    if(it == timers_.end() || when < it->first)
    {
        earliestChanged = true;
    }

    timers_.insert(Entry(when, timer));
    activeTimers_.insert(ActiveTimer(timer, timer->sequence()));

    return earliestChanged;
}

void TimerQueue::cancel(TimerId timerId)
{
    loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    loop_->assertInLoopThread();

    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    auto it = activeTimers_.find(timer);
    if(it != activeTimers_.end()) //定时器尚未到期取消定时器
    {
        timers_.erase(Entry(it->first->expiration(), it->first));
        activeTimers_.erase(timer);
    }
    else if(callingExpiredTimers_)  //定时器到期之后执行回调取消定时器
    {
        cancelingTimers_.insert(timer);
    }
}

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();

    Timestamp now(Timestamp::now());
    readTimerfd();

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();

    for(auto& entry : expired)
    {
        entry.second->run();
    }

    callingExpiredTimers_ = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    //Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    Entry sentry(now, TimerPtr());
    auto end = timers_.lower_bound(sentry);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    for(auto& entry:expired)
    {
        ActiveTimer timer(entry.second, entry.second->sequence());
        activeTimers_.erase(timer);
    }

    return expired;
}

void TimerQueue::reset(std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpired;

    for(auto& entry : expired)
    {
        ActiveTimer timer(entry.second, entry.second->sequence());

        if(entry.second->repeat()
           && cancelingTimers_.find(timer) == cancelingTimers_.end())
        {
            entry.second->restart(now);
            insert(entry.second);
        }
        else
        {
            entry.second.reset();
        }
    }

    if(!timers_.empty())
    {
        nextExpired = timers_.begin()->second->expiration();
    }

    if(nextExpired.valid())
    {
        resetTimerfd(nextExpired);
    }
}