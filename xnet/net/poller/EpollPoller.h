//
// Created by zhangkuo on 17-8-12.
//

#ifndef WEBSERVER_EPOLLPOLLER_H
#define WEBSERVER_EPOLLPOLLER_H
#include "../Poller.h"
#include <sys/epoll.h>
#include <vector>

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

    using EventList = std::vector<struct epoll_event>;

    int epollfd_;
    EventList events_;
};

}

#endif //WEBSERVER_EPOLLPOLLER_H
