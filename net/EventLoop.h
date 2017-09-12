//
// Created by zhangkuo on 17-8-6.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include <boost/noncopyable.hpp>
#include "Channel.h"
#include "boost/scoped_ptr.hpp"
#include "Poller.h"
#include "../base/Timestamp.h"
#include "../base/Mutex.h"
#include "../base/CurrentThread.h"
#include "Timer.h"
#include "TimerId.h"

namespace WebServer
{
class Channel;
class Poller;
class TimerQueue;

class EventLoop : boost::noncopyable
{
public:
    using Functor = std::function<void()>;

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


private:
    void handleRead();
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;
    int64_t iteration_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    boost::scoped_ptr<Poller> poller_;
    boost::scoped_ptr<TimerQueue> timerQueue_;
    int wakeupFd_;
    boost::scoped_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
};

}

#endif //WEBSERVER_EVENTLOOP_H
