//
// Created by zhangkuo on 17-9-15.
//
#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include <boost/implicit_cast.hpp>

using namespace WebServer;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, const std::string& name):
        baseLoop_(loop),
        name_(name),
        started_(false),
        numThreads_(0),
        next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    started_ = true;
    for(int i = 0; i <numThreads_; i++)
    {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", name_.data(), i);
        EventLoopThread* t = new EventLoopThread(cb, buf);
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
    }

    if(numThreads_ == 0)
    {
        cb(baseLoop_);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop* loop = baseLoop_;
    if(!loops_.empty())
    {
        loop = loops_[next_];
        ++next_;
        if(boost::implicit_cast<size_t>(next_) > loops_.size())
        {
            next_ = 0;
        }
    }

    return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
    EventLoop* loop = baseLoop_;
    if(!loops_.empty())
    {
        loop = loops_[hashCode % loops_.size()];
    }

    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    if(loops_.empty())
    {
        return std::vector<EventLoop*>(1, baseLoop_);
    }
    else
    {
        return loops_;
    }
}
