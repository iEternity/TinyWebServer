//
// Created by zhangkuo on 17-9-13.
//

#ifndef WEBSERVER_EVENTLOOPTHREAD_H
#define WEBSERVER_EVENTLOOPTHREAD_H

#include <boost/noncopyable.hpp>
#include "../base/Thread.h"
#include "../base/Mutex.h"
#include "../base/Condition.h"

namespace xnet
{

class EventLoop;

class EventLoopThread : boost::noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                    const std::string& name = std::string());
    ~EventLoopThread();

    EventLoop* startLoop();
private:
    void threadFunc();

    EventLoop* loop_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    ThreadInitCallback callback_;
};

}

#endif //WEBSERVER_EVENTLOOPTHREAD_H
