//
// Created by zhangkuo on 17-9-13.
//
#include "EventLoopThread.h"
#include "EventLoop.h"

using namespace WebServer;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
                                 const std::string& name):
        loop_(nullptr),
        thread_(std::bind(&EventLoopThread::threadFunc, this), name),
        mutex_(),
        cond_(mutex_),
        callback_(cb)

{

}

EventLoopThread::~EventLoopThread()
{
    if(loop_ != nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    thread_.start();

    {
        MutexLockGuard lock(mutex_);
        while(loop_ == nullptr)
        {
            cond_.wait();
        }
    }

    return loop_;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    if(callback_) callback_(&loop);

    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    loop_ = nullptr;
}