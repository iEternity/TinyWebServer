//
// Created by zhangkuo on 17-11-5.
//

#ifndef XNET_ASYNCLOGGING_H
#define XNET_ASYNCLOGGING_H
#include <xnet/base/noncopyable.h>
#include <xnet/base/Types.h>
#include <xnet/base/LogStream.h>
#include <memory>
#include <vector>
#include <xnet/base/Thread.h>
#include <xnet/base/CountDownLatch.h>

namespace xnet
{

class AsyncLogging : noncopyable
{
    using FixedBuffer = xnet::detail::FixedBuffer<xnet::detail::kLargeBuffer>;
    using BufferPtr = std::unique_ptr<FixedBuffer>;
    using BufferVector = std::vector<BufferPtr>;

public:
    AsyncLogging(const string& basename, size_t rollSize, int flushInterval = 3);
    ~AsyncLogging()
    {
        if(running_)
        {
            stop();
        }
    }

    void append(const char* logline, size_t len);

    void start()
    {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop()
    {
        running_ = false;
        cond_.notify_one();
        thread_.join();
    }

private:
    void threadFunc();

private:
    const string    basename_;
    const size_t    rollSize_;
    const int       flushInterval_;

    BufferPtr       currentBuffer_;
    BufferPtr       nextBuffer_;
    BufferVector    buffers_;

    bool            running_;
    Thread          thread_;
    CountDownLatch  latch_;

    std::mutex                  mutex_;
    std::condition_variable     cond_;
};

}

#endif //XNET_ASYNCLOGGING_H
