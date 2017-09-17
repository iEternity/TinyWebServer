//
// Created by zhangkuo on 17-8-18.
//

#ifndef WEBSERVER_MUTEX_H
#define WEBSERVER_MUTEX_H
#include <boost/noncopyable.hpp>
#include <pthread.h>
#include "CurrentThread.h"
#include <assert.h>

namespace WebServer
{

class MutexLock
{
public:
    MutexLock()
        : holder_(0)
    {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock()
    {
        assert(holder_ == 0);
        pthread_mutex_destroy(&mutex_);
    }

    bool isLockedByThisThread() const
    {
        return holder_ == CurrentThread::tid();
    }

    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;
    }

private:
    friend class Condition;

    class UnassignGuard : boost::noncopyable
    {
    public:
        UnassignGuard(MutexLock& owner):
                owner_(owner)
        {
            owner_.unassignHolder();
        }

        ~UnassignGuard()
        {
            owner_.assignHolder();
        }
    private:
        MutexLock& owner_;
    };

    void assignHolder()
    {
        holder_ = CurrentThread::tid();
    }

    void unassignHolder()
    {
        holder_ = 0;
    }

    pthread_mutex_t mutex_;
    pid_t holder_;
};

class MutexLockGuard : boost::noncopyable
{
public:
    explicit MutexLockGuard(MutexLock& mutex)
        : mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock mutex_;
};

}

#define MutexLockGuard(x) error "Missing guard object name"
#endif //WEBSERVER_MUTEX_H
