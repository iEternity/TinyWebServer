//
// Created by zhangkuo on 17-8-23.
//
#include <xnet/net/Timer.h>
using namespace xnet;

std::atomic<int64_t> Timer::s_numCreated_;

void Timer::restart(Timestamp now)
{
    if (repeat_)
    {
        expiration_ = addTime(now, interval_);
    }
    else
    {
        expiration_ = Timestamp::invalid();
    }
}