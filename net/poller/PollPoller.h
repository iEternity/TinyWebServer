//
// Created by zhangkuo on 17-8-9.
//

#ifndef WEBSERVER_POLLPOLLER_H
#define WEBSERVER_POLLPOLLER_H

#include "../Poller.h"
#include "poll.h"

namespace WebServer
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

    using PollFdList = std::vector<struct pollfd>;
    PollFdList pollfds_;
};

}


#endif //WEBSERVER_POLLPOLLER_H
