//
// Created by zhangkuo on 17-11-6.
//
#include <xnet/base/CountDownLatch.h>

using namespace xnet;

CountDownLatch::CountDownLatch(int count):
        count_(count)
{

}

void CountDownLatch::wait()
{
    std::unique_lock lock(mutex_);
    while(count_ > 0)
    {
        condition_.wait(lock);
    }
}

void CountDownLatch::countDown()
{
    std::unique_lock lock(mutex_);
    count_ --;
    if(count_ == 0)
    {
        condition_.notify_all();
    }
}

int CountDownLatch::getCount() const
{
    std::unique_lock lock(mutex_);
    return count_;
}