//
// Created by zhangkuo on 17-8-9.
//

#include <xnet/net/Poller.h>
using namespace xnet;

Poller::Poller(EventLoop* loop)
    : ownerLoop_(loop)
{
}

Poller::~Poller()
{

}

bool Poller::hasChannel(Channel* channel) const
{
    auto it = channels_.find(channel->fd());

    return it != channels_.end() && it->second == channel;
}