//
// Created by zhangkuo on 17-8-13.
//
#include "../Poller.h"
#include "PollPoller.h"
#include "EpollPoller.h"
using namespace xnet;

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    if(::getenv("WEBSERVER_USE_POLL"))
    {
        return new PollPoller(loop);
    }
    else
    {
        return new EpollPoller(loop);
    }
}
