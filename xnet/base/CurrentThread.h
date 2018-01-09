//
// Created by zhangkuo on 17-8-20.
//

#ifndef XNET_CURRENTTHREAD_H
#define XNET_CURRENTTHREAD_H
#include <string>
#include <sys/syscall.h>
#include <thread>
#include <chrono>
#include <unistd.h>

namespace xnet
{

namespace CurrentThread
{
    extern thread_local int t_cachedTid;
    extern thread_local std::string t_tidString;
    extern thread_local std::string t_threadName;

    static pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }


    inline int tid()
    {
        if (t_cachedTid == 0)
        {
            t_cachedTid = gettid();
            t_tidString = std::to_string(t_cachedTid);
        }
        return t_cachedTid;
    }

    inline const std::string& tidString()
    {
        return t_tidString;
    }

    inline const std::string& name()
    {
        return t_threadName;
    }

    inline bool isMainThread()
    {
        return t_cachedTid == ::getpid();
    }

    inline void sleep(int64_t sec)
    {
        std::this_thread::sleep_for(std::chrono::seconds(sec));
    }

    inline void sleepMsec(int64_t msec)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(msec));
    }

    inline void sleepUsec(int64_t usec)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(usec));
    }

}   //namespace CurrentThread

}   //namespace xNet

#endif //XNET_CURRENTTHREAD_H
