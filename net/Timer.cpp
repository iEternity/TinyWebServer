//
// Created by zhangkuo on 17-8-23.
//
#include "Timer.h"
using namespace WebServer;

AtomicInt64 Timer::s_numCreated_;

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