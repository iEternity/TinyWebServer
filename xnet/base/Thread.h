//
// Created by zhangkuo on 17-8-18.
//

#ifndef XNET_THREAD_H
#define XNET_THREAD_H

#include <thread>
#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <xnet/base/noncopyable.h>
#include <xnet/base/CountDownLatch.h>

namespace xnet
{

class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

public:
    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    explicit Thread(ThreadFunc&& , const std::string & name = std::string());

    ~Thread();

    void start();
    void join();

    bool started() const { return started_; }
    const std::string& name() const { return name_; }
    pid_t tid() const { return tid_; }

private:
    void setDefaultName();

    void runInThread();

private:
    ThreadFunc  func_;
    std::string name_;
    bool        started_;
    bool        joined_;
    std::thread thread_;
    pid_t       tid_;

    CountDownLatch latch_;

    static std::atomic<int> numCreated_;
};

}

#endif //XNET_THREAD_H
