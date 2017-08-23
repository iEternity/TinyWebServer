//
// Created by zhangkuo on 17-8-18.
//

#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H

#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <functional>
#include <string>
#include <memory>
#include "Atomic.h"

namespace WebServer
{

class Thread : boost::noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    explicit Thread(ThreadFunc&& , const std::string & name = std::string());

    ~Thread();

    void start();
    int join(); //return pthread_join()

    bool started() const { return started_; }
    const std::string& name() const { return name_; }

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    std::shared_ptr<pid_t> tid_;
    ThreadFunc func_;
    std::string name_;

    static AtomicInt32 numCreated_;
};

}

#endif //WEBSERVER_THREAD_H
