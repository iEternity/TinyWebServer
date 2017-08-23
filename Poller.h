//
// Created by zhangkuo on 17-8-9.
//

#ifndef WEBSERVER_POLLER_H
#define WEBSERVER_POLLER_H
#include <boost/noncopyable.hpp>
#include <vector>
#include <map>
#include "Channel.h"
#include "EventLoop.h"
#include "base/Timestamp.h"

namespace WebServer
{

class Channel;
class EventLoop;

class Poller : boost::noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    virtual ~Poller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

    virtual void updateChannel(Channel* channel) = 0;

    virtual void removeChannel(Channel* channel) = 0;

    virtual bool hasChannel(Channel* channel) const;

    static Poller* newDefaultPoller(EventLoop* loop);

protected:
    using ChannelMap = std::map<int, Channel*>;
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_;
};

}

#endif //WEBSERVER_POLLER_H