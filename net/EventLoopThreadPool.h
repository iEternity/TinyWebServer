//
// Created by zhangkuo on 17-9-14.
//

#ifndef WEBSERVER_EVENTLOOPTHREADPOOL_H
#define WEBSERVER_EVENTLOOPTHREADPOOL_H


#include <boost/noncopyable.hpp>
#include <functional>
#include <boost/ptr_container/ptr_vector.hpp>

namespace WebServer
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : boost::noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop* loop, const std::string& name);
    ~EventLoopThreadPool();

    void setThreadNum(int threadNum)
    { numThreads_ = threadNum; }

    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop* getNextLoop();
    EventLoop* getLoopForHash(size_t hashCode);

    std::vector<EventLoop*> getAllLoops();

    bool started() const { return started_; }

    const std::string& name() const {return name_; }
private:
    EventLoop* baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    boost::ptr_vector<EventLoopThread> threads_;
    std::vector<EventLoop*> loops_;
};

}

#endif //WEBSERVER_EVENTLOOPTHREADPOOL_H
