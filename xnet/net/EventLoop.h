//
// Created by zhangkuo on 17-8-6.
//

#ifndef XNET_EVENTLOOP_H
#define XNET_EVENTLOOP_H

#include <mutex>
#include <vector>
#include <boost/any.hpp>
#include <xnet/base/noncopyable.h>
#include <xnet/base/Timestamp.h>
#include <xnet/base/CurrentThread.h>
#include <xnet/net/Timer.h>
#include <xnet/net/TimerId.h>
#include <xnet/net/Channel.h>
#include <xnet/net/Callbacks.h>

namespace xnet
{
class Channel;
class Poller;
class TimerQueue;

class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;
    using ChannelList = std::vector<Channel*>;
    using MutexLockGuard = std::lock_guard<std::mutex>;

public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    // internal usage
    void wakeup();
    bool hasChannel(Channel* channel);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void runInLoop(const Functor& cb);
    void runInLoop(Functor&& cb);
    void queueInLoop(const Functor& cb);
    void queueInLoop(Functor&& cb);

    TimerId runAt(const Timestamp& time, const TimerCallback& cb);
    TimerId runAt(const Timestamp& time, TimerCallback&& cb);
    TimerId runAfter(double delay, const TimerCallback& cb);
    TimerId runAfter(double delay, TimerCallback&& cb);
    TimerId runEvery(double interval, const TimerCallback& cb);
    TimerId runEvery(double interval, TimerCallback&& cb);

    void cancel(TimerId timerId);

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
    void assertInLoopThread() const;

    bool isEventHandling() const { return eventHandling_; }

    int64_t getIteration() const { return iteration_; }

    Timestamp getPollReturnTime() const { return pollReturnTime_; }

private:
    void handleRead();
    void doPendingFunctors();

    void abortNotInLoopThread() const;

private:
    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;

    int64_t     iteration_;
    const pid_t threadId_;
    Timestamp   pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    std::mutex mutex_;
    std::vector<Functor> pendingFunctors_;

    boost::any context_;
};

}

#endif //XNET_EVENTLOOP_H
