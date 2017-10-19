//
// Created by zhangkuo on 17-10-15.
//

#ifndef XNET_THREADPOOL_H
#define XNET_THREADPOOL_H
#include <boost/noncopyable.hpp>
#include <functional>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include "Types.h"
#include "Thread.h"

namespace xnet
{

class ThreadPool : boost::noncopyable
{
    using Task = std::function<void()>;
    using ThreadPtr = std::shared_ptr<Thread>;
    using UniqueLock = std::unique_lock<std::mutex>;
public:
    explicit ThreadPool(const string& nameArg = string("ThreadPool"));
    ~ThreadPool();

    void start(int numThread);
    void stop();
    void run(const Task& task);
    void run(Task&& task);
    void setMaxQueueSize(int maxSize);
    const string& name() const { return name_; }

private:
    void runInThread();
    bool isFull() const;
    Task take();

    string                      name_;
    std::vector<ThreadPtr>      threads_;
    std::deque<Task>            queue_;
    mutable std::mutex          mutex_;
    std::condition_variable     notEmpty_;
    std::condition_variable     notFull_;
    bool                        running_;
    int                         maxQueueSize_;
};

}

#endif //XNET_THREADPOOL_H
