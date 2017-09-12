//
// Created by zhangkuo on 17-8-24.
//

#ifndef WEBSERVER_TIMERID_H
#define WEBSERVER_TIMERID_H
#include "Timer.h"

namespace WebServer
{

class Timer;
class TimerId
{
public:
    using TimerPtr = std::shared_ptr<Timer>;

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

#endif //WEBSERVER_TIMERID_H
