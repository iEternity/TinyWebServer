//
// Created by zhangkuo on 17-8-18.
//
#include <sys/prctl.h>
#include <sstream>
#include <iostream>
#include <xnet/base/Thread.h>
#include <xnet/base/CurrentThread.h>
#include <xnet/base/Timestamp.h>
#include <xnet/base/Exception.h>

namespace xnet
{

namespace CurrentThread
{
    thread_local int t_cachedTid = 0;
    thread_local std::string t_tidString = "0";
    thread_local std::string t_threadName = "unknown";

    // fork 之后清空已经缓存的线程信息
    void afterFork()
    {
        t_cachedTid = 0;
        t_threadName = "main";
        tid();
    }

    int init()
    {
        tid();
        t_threadName = "main";
        pthread_atfork(NULL, NULL, &afterFork);

        return 0;
    }
}  //namespace CurrentThread
}   //namespace xnet


int dummy = xnet::CurrentThread::init();

using namespace xnet;
using namespace std;

atomic<int> Thread::numCreated_ = { 0 };

Thread::Thread(const ThreadFunc& func, const std::string& name)
        : func_(func),
          name_(name),
          started_(false),
          joined_(false),
          tid_(0),
          latch_(1)
{
    setDefaultName();
}

Thread::Thread(ThreadFunc&& func, const std::string& name)
        : func_(std::move(func)),
          name_(name),
          started_(false),
          joined_(false),
          tid_(0),
          latch_(1)

{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        thread_.detach();
    }
}

void Thread::setDefaultName()
{
    numCreated_++;
    if (name_.empty())
    {
        std::ostringstream oss;
        oss << "Thread" << numCreated_;
        name_ = oss.str();
    }
}

void Thread::start()
{
    assert(!started_);

    started_ = true;
    thread_ = std::thread(std::bind(&Thread::runInThread, this));
    latch_.wait();
}

void Thread::join()
{
    assert(started_);
    assert(!joined_);

    joined_ = true;
    thread_.join();
}

void Thread::runInThread()
{
    latch_.countDown();

    tid_ = CurrentThread::tid();
    CurrentThread::t_threadName = name_.empty() ? "xNetThread" : name_;
    ::prctl(PR_SET_NAME, CurrentThread::t_threadName);

    try
    {
        func_();
        CurrentThread::t_threadName = "finished";
    }
    catch(const Exception& ex)
    {
        CurrentThread::t_threadName = "crashed";
        cerr << "exception caught in Thread " << name_ << endl;
        cerr << "reason: " << ex.what() << endl;
        cerr << "stack trace: " << ex.stackTrace() << endl;

        ::abort();
    }
    catch(const std::exception& ex)
    {
        CurrentThread::t_threadName = "crashed";
        cerr << "exception caught in Thread " << name_ << endl;
        cerr << "reason: " << ex.what() << endl;

        ::abort();
    }
    catch(...)
    {
        CurrentThread::t_threadName = "crashed";
        cerr << "exception caught in Thread " << name_ << endl;

        throw;
    }
}