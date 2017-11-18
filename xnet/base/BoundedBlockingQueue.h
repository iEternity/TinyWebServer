//
// Created by zhangkuo on 17-11-18.
//

#ifndef XNET_BOUNDEDBLOCKINGQUEUE_H
#define XNET_BOUNDEDBLOCKINGQUEUE_H
#include <queue>
#include <mutex>
#include <condition_variable>
#include <xnet/base/noncopyable.h>

namespace xnet
{

template<typename T>
class BoundedBlockingQueue : noncopyable
{
    using UniqueLock = std::unique_lock<std::mutex>;
public:
    BoundedBlockingQueue(int maxSize):
        maxSize_(maxSize)
    {

    }

    void put(const T& data)
    {
        UniqueLock lock(mutex_);
        while(queue_.size() >= maxSize_)
        {
            notFull_.wait(lock);
        }

        queue_.push(data);
        notEmpty_.notify_one();
    }

    void put(T&& data)
    {
        UniqueLock lock(mutex_);
        while(queue_.size() >= maxSize_)
        {
            notFull_.wait(lock);
        }

        queue_.push(std::move(data));
        notEmpty_.notify_one();
    }

    T take()
    {
        UniqueLock lock(mutex_);
        while(queue_.empty())
        {
            notEmpty_.wait(lock);
        }

        T data = std::move(queue_.front());
        queue_.pop();

        notFull_.notify_one();

        return data;
    }

    size_t size() const
    {
        UniqueLock lock(mutex_);
        return queue_.size();
    }

private:
    const int           maxSize_;
    std::queue<T>           queue_;
    mutable std::mutex              mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

}

#endif //XNET_BOUNDEDBLOCKINGQUEUE_H
