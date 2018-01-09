//
// Created by zhangkuo on 17-8-24.
//

#ifndef XNET_TIMERID_H
#define XNET_TIMERID_H

namespace xnet
{

class Timer;
class TimerId
{
public:
    using TimerPtr = std::shared_ptr<Timer>;

public:
    TimerId()
        : timer_(nullptr),
          sequence_(0)
    {
    }

    TimerId(TimerPtr timer, int64_t seq)
        : timer_(timer),
          sequence_(seq)
    {
    }

    friend class TimerQueue;
private:
    TimerPtr timer_;
    int64_t sequence_;
};


}

#endif //XNET_TIMERID_H
