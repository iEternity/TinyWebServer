//
// Created by zhangkuo on 17-8-24.
//

#ifndef XNET_TIMERQUEUE_H
#define XNET_TIMERQUEUE_H
#include <boost/noncopyable.hpp>
#include <set>
#include "EventLoop.h"
#include "TimerId.h"
#include "Callbacks.h"
#include "Channel.h"

namespace xnet
{
class EventLoop;
class TimerId;

class TimerQueue : boost::noncopyable
{
public:
    TimerQueue(EventLoop* loop);

    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);
    TimerId addTimer(TimerCallback&& cb, Timestamp when, double interval);

    void cancel(TimerId timerId);
private:
    using TimerPtr = std::shared_ptr<Timer>;
    using Entry = std::pair<Timestamp, TimerPtr>;
    using TimerList = std::set<Entry>;
    using ActiveTimer = std::pair<TimerPtr, int64_t>;
    using ActiveTimerSet = std::set<ActiveTimer>;

    void addTimerInLoop(TimerPtr timer);
    void cancelInLoop(TimerId timerId);
    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);
    void reset(std::vector<Entry>& expired, Timestamp now);

    bool insert(TimerPtr timer);

    EventLoop* loop_;
    const int timerfd_;
    Channel timerfdChannel_;

    /*Timer list sorted by expiration*/
    TimerList timers_;

    /*For cancel*/
    ActiveTimerSet activeTimers_;
    bool callingExpiredTimers_;
    ActiveTimerSet cancelingTimers_;
};

}

#endif //XNET_TIMERQUEUE_H
