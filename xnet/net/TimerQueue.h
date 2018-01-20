//
// Created by zhangkuo on 17-8-24.
//

#ifndef XNET_TIMERQUEUE_H
#define XNET_TIMERQUEUE_H
#include <set>
#include <xnet/base/noncopyable.h>
#include <xnet/net/EventLoop.h>
#include <xnet/net/Callbacks.h>
#include <xnet/net/Channel.h>

namespace xnet
{
class EventLoop;
class TimerId;
class Timer;

class TimerQueue :noncopyable
{
public:
    using TimerPtr          = std::shared_ptr<Timer>;
    using Entry             = std::pair<Timestamp, TimerPtr>;
    using TimerList         = std::set<Entry>;
    using ActiveTimer       = std::pair<TimerPtr, int64_t>;
    using ActiveTimerSet    = std::set<ActiveTimer>;

public:
    TimerQueue(EventLoop* loop);

    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);
    TimerId addTimer(TimerCallback&& cb, Timestamp when, double interval);

    void cancel(TimerId timerId);

private:
    static int createTimerfd();
    void readTimerfd() const;
    void resetTimerfd(Timestamp expiration);

    void handleRead();
    void addTimerInLoop(TimerPtr timer);
    void cancelInLoop(TimerId timerId);
    
    std::vector<Entry> getExpired(Timestamp now);

    // 重新启动过期的定时器中的周期性定时器，并重置timerfd
    void reset(std::vector<Entry>& expired, Timestamp now);

    bool insert(TimerPtr timer);

private:
    EventLoop*  loop_;
    const int   timerfd_;
    Channel     timerfdChannel_;

    /*Timer list sorted by expiration*/
    TimerList timers_;

    /*For cancel*/
    ActiveTimerSet activeTimers_;   //便于根据TimerId找到定时器
    bool callingExpiredTimers_;     //定时器到期后正在执行回调
    /*
     * 定时器到期之后正在执行回调，在回调中取消当前定时器，将这样的定时器记录下来
     * 便于在reset函数中将这样的周期性定时器不必执行restart
     */
    ActiveTimerSet cancelingTimers_;
};

}

#endif //XNET_TIMERQUEUE_H
