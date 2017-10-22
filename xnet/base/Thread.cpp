//
// Created by zhangkuo on 17-8-18.
//
#include "Thread.h"
#include "CurrentThread.h"
#include "Timestamp.h"
#include <sys/syscall.h>
#include <sys/prctl.h>

namespace xnet
{

namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "unkonwn";
}

namespace detail
{

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork()
{
    CurrentThread::t_cachedTid = 0;
    CurrentThread::t_threadName = "main";
    CurrentThread::cacheTid();
}

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }

};

ThreadNameInitializer init;

struct ThreadData
{
    using ThreadFunc = Thread::ThreadFunc;
    using WkTid = std::weak_ptr<pid_t>;

    ThreadFunc func_;
    std::string name_;
    WkTid wkTid_;

    ThreadData(const ThreadFunc& func, const std::string name, const WkTid& tid)
            : func_(func),
              name_(name),
              wkTid_(tid)
    {
    }

    void runInThread()
    {
        pid_t tid = CurrentThread::tid();

        std::shared_ptr<pid_t> pTid = wkTid_.lock();
        if (pTid)
        {
            *pTid = tid;
            pTid.reset();
        }

        xnet::CurrentThread::t_threadName = name_.empty() ? "webServerThread" : name_.c_str();
        ::prctl(PR_SET_NAME, CurrentThread::t_threadName);

        func_();
        CurrentThread::t_threadName = "finished";
    }
};

void* startThread(void* obj)
{
    ThreadData* pData = static_cast<ThreadData*>(obj);
    pData->runInThread();

    delete pData;
    return nullptr;
}

}   //namespace detail
}   //namespace xNet

using namespace xnet;

void CurrentThread::cacheTid()
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    }
}

bool CurrentThread::isMainThread()
{
    return t_cachedTid == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicrosecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicrosecondsPerSecond * 1000);

    ::nanosleep(&ts, NULL);
}

AtomicInt32 Thread::numCreated_;
Thread::Thread(const ThreadFunc& func, const std::string& name)
        : started_(false),
          joined_(false),
          pthreadId_(0),
          tid_(new pid_t(0)),
          func_(func),
          name_(name)
{
    setDefaultName();
}

Thread::Thread(ThreadFunc&& func, const std::string& name)
        : started_(false),
          joined_(false),
          pthreadId_(0),
          tid_(new pid_t(0)),
          func_(std::move(func)),
          name_(name)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = numCreated_.incrementAndGet();
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}

void Thread::start()
{
    started_ = true;
    detail::ThreadData* pData = new detail::ThreadData(func_, name_, tid_);
    if (pthread_create(&pthreadId_, NULL, detail::startThread, pData))
    {
        started_ = false;
        delete pData;
    }
}

int Thread::join()
{
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}

