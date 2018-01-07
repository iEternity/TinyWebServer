//
// Created by zhangkuo on 17-8-9.
//

#pragma once

#include <poll.h>
#include <xnet/net/Poller.h>

namespace xnet
{

class PollPoller : public Poller
{
public:
    PollPoller(EventLoop* loop);
    virtual ~PollPoller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
    virtual void updateChannel(Channel* channel);
    virtual void removeChannel(Channel* channel);

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

private:
    std::vector<struct pollfd> pollfds_;
};

}

