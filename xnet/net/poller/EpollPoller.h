//
// Created by zhangkuo on 17-8-12.
//

#pragma once
#include <sys/epoll.h>
#include <vector>
#include <xnet/net/Poller.h>

namespace xnet
{

class EpollPoller : public Poller
{
public:
    EpollPoller(EventLoop* loop);
    virtual ~EpollPoller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
    virtual void updateChannel(Channel* channel);
    virtual void removeChannel(Channel* channel);

private:
    static const int kInitEventListSize = 16;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    void update(int operation, Channel* channel);

private:
    int epollfd_;
    std::vector<struct epoll_event> events_;
};

}
