//
// Created by zhangkuo on 17-9-13.
//

#ifndef WEBSERVER_CONDITION_H
#define WEBSERVER_CONDITION_H

#include <boost/noncopyable.hpp>
#include "Mutex.h"
#include <pthread.h>

namespace WebServer
{

class Condition : boost ::noncopyable
{
public:
    explicit Condition(MutexLock& mutex)
        : mutex_(mutex)
    {
        ::pthread_cond_init(&pcond_, NULL);
    }

    ~Condition()
    {
        ::pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        MutexLock::UnassignGuard ug(mutex_);
        ::pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    void waitForSeconds(double seconds);

    void notify()
    {
        ::pthread_cond_signal(&pcond_);
    }

    void notifyAll()
    {
        ::pthread_cond_broadcast(&pcond_);
    }

private:
    MutexLock mutex_;
    pthread_cond_t pcond_;
};

}

#endif //WEBSERVER_CONDITION_H
