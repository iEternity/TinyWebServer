//
// Created by zhangkuo on 17-8-9.
//

#include "Poller.h"
using namespace WebServer;

Poller::Poller(EventLoop* loop)
    : ownerLoop_(loop)
{
}

Poller::~Poller()
{

}

bool Poller::hasChannel(Channel* channel) const
{
    ChannelMap::const_iterator it = channels_.find(channel->fd());

    return it != channels_.end() && it->second == channel;
}