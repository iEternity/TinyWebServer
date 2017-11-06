//
// Created by zhangkuo on 17-11-6.
//

#ifndef XNET_COUNTDOWNLATCH_H
#define XNET_COUNTDOWNLATCH_H

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace xnet
{

class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    std::mutex              mutex_;
    int                     count_;
    std::condition_variable condition_;
};

}

#endif //XNET_COUNTDOWNLATCH_H
