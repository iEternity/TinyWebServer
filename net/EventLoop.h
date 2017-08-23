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

namespace WebServer
{
class Channel;
class Poller;

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
    bool hasChannel();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void runInLoop(const Functor& cb);
    void runInLoop(Functor&& cb);
    void queueInLoop(const Functor& cb);
    void queueInLoop(Functor&& cb);

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }


private:
    void handleRead();
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;

    const pid_t threadId_;

    int wakeupFd_;
    boost::scoped_ptr<Channel> wakeupChannel_;
    boost::scoped_ptr<Poller> poller_;

    ChannelList activeChannels_;
    Timestamp pollReturnTime_;
    Channel* currentActiveChannel_;

    MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
};

}

#endif //WEBSERVER_EVENTLOOP_H
