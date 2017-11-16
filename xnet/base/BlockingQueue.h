//
// Created by zhangkuo on 17-11-16.
//

#ifndef XNET_BLOCKINGQUEUE_H
#define XNET_BLOCKINGQUEUE_H
#include <xnet/base/noncopyable.h>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace xnet
{

template<typename T>
class BlockingQueue : noncopyable
{
public:
    BlockingQueue() = default;
    ~BlockingQueue() = default;

    void put(const T& task)
    {
        std::unique_lock<mutex> lock(mutex_);
        queue_.push(task);
        notEmpty_.notify_one();
    }

    void put(T&& task)
    {
        std::unique_lock<mutex> lock(mutex_);
        queue_.push_back(std::move(task));
        notEmpty_.notify_one();
    }

    T take()
    {
        std::unique_lock<mutex> lock(mutex_);
        while(queue_.empty())
        {
            notEmpty_.wait(lock);
        }
        T task = std::move(queue_.front());
        queue_.pop();

        return task;
    }

    size_t size() const
    {
        std::unique_lock lock(mutex_);
        return queue_.size();
    }

private:
    mutable std::mutex      mutex_;
    std::queue<T>           queue_;
    std::condition_variable notEmpty_;
};

}

#endif //XNET_BLOCKINGQUEUE_H
