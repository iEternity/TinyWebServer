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
    TimerId()
        : timer_(NULL),
          sequence_(0)
    {
    }

    TimerId(Timer* timer, int64_t seq)
        : timer_(timer),
          sequence_(seq)
    {
    }

    friend class TimerQueue;
private:
    Timer* timer_;
    int64_t sequence_;
};


}

#endif //WEBSERVER_TIMERID_H
