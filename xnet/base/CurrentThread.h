//
// Created by zhangkuo on 17-8-20.
//

#ifndef WEBSERVER_CURRENTTHREAD_H
#define WEBSERVER_CURRENTTHREAD_H

namespace xnet
{

namespace CurrentThread
{
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char* t_threadName;

    void cacheTid();

    inline int tid()
    {
        if (t_cachedTid == 0)
        {
            cacheTid();
        }
        return t_cachedTid;
    }

    inline const char* tidString()
    {
        return t_tidString;
    }

    inline size_t tidStringLength()
    {
        return static_cast<size_t>(t_tidStringLength);
    }

    inline const char* name()
    {
        return t_threadName;
    }

    bool isMainThread();

    void sleepUsec(int64_t usec);
}   //namespace CurrentThread

}   //namespace xNet

#endif //WEBSERVER_CURRENTTHREAD_H
