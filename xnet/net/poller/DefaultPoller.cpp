//
// Created by zhangkuo on 17-8-13.
//
#include <xnet/net/Poller.h>
#include <xnet/net/poller/PollPoller.h>
#include <xnet/net/poller/EpollPoller.h>
using namespace xnet;

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    if(::getenv("XNET_USE_POLL"))
    {
        return new PollPoller(loop);
    }
    else
    {
        return new EpollPoller(loop);
    }
}
