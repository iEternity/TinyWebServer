//
// Created by zhangkuo on 17-8-25.
//
#include <string.h>
#include "TimerQueue.h"
#include "sys/timerfd.h"

namespace WebServer
{

namespace detail
{

int createTimerfd()
{
    int timerFd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    return timerFd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.microsecondsSinceEpoch() - Timestamp::now().microsecondsSinceEpoch();
    if(microseconds < 100) microseconds = 100;

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicrosecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicrosecondsPerSecond) * 1000);

    return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howMany;
    ::read(timerfd, &howMany, sizeof howMany);
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec oldValue;
    struct itimerspec newValue;
    bzero(&oldValue, sizeof oldValue);
    bzero(&newValue, sizeof newValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    timerfd_settime(timerfd, 0, &newValue, & oldValue);
}

}

}

using namespace WebServer;
using namespace WebServer::detail;

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

//    for(auto it:timers_)
//    {
//        delete it.second;
//    }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
    TimerPtr timer = std::make_shared<Timer>(cb, when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer.get(), timer->sequence());
}

TimerId TimerQueue::addTimer(TimerCallback&& cb, Timestamp when, double interval)
{
    TimerPtr timer = std::make_shared<Timer>(std::move(cb), when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer.get(), timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
    loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(TimerPtr timer)
{
    bool earliestChanged = insert(timer);

    if(earliestChanged)
    {
        resetTimerfd(timerfd_, timer->expiration());
    }
}


void TimerQueue::cancelInLoop(TimerId timerId)
{
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    auto it = activeTimers_.find(timer);
    if(it != activeTimers_.end())
    {
        timers_.erase(Entry(it->first->expiration(), it->first));
        it->first.reset();
        activeTimers_.erase(timer);
    }
    else if(callingExpiredTimers_)
    {
        cancelingTimers_.insert(timer);
    }
}

void TimerQueue::handleRead()
{
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();

    for(auto entry:expired)
    {
        entry.second->run();
    }

    callingExpiredTimers_ = false;

}

std::vector<Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    //Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    Entry sentry(now, TimerPtr(reinterpret_cast<Timer*>(UINTPTR_MAX)));
    auto end = timers_.lower_bound(sentry);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    for(auto entry:expired)
    {
        ActiveTimer timer(entry.second, entry.second->sequence());
    }

    return expired;
}

void TimerQueue::reset(std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpired;

    for(auto entry : expired)
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
        resetTimerfd(timerfd_, nextExpired);
    }
}

bool TimerQueue::insert(TimerPtr timer)
{
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if(it == timers_.end() || when < it->first)
    {
        earliestChanged = true;
    }

    timers_.insert(Entry(when, timer));
    activeTimers_.insert(ActiveTimer(timer, timer->sequence()));

    return earliestChanged;
}