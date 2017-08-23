//
// Created by zhangkuo on 17-8-18.
//

#ifndef WEBSERVER_MUTEX_H
#define WEBSERVER_MUTEX_H
#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace WebServer
{

class MutexLock
{
public:
    MutexLock()
    {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock()
    {
        pthread_mutex_destroy(&mutex_);
    }

    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

private:
    pthread_mutex_t mutex_;
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

#endif //WEBSERVER_MUTEX_H
