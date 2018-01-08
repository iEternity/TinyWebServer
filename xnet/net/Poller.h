//
// Created by zhangkuo on 17-8-9.
//

#pragma once
#include <boost/noncopyable.hpp>
#include <vector>
#include <map>
#include <xnet/net/Channel.h>
#include <xnet/net/EventLoop.h>
#include <xnet/base/Timestamp.h>

namespace xnet
{

class Channel;
class EventLoop;

class Poller : boost::noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;
    using ChannelMap  = std::map<int, Channel*>;

public:
    Poller(EventLoop* loop);
    virtual ~Poller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

    virtual void updateChannel(Channel* channel) = 0;

    virtual void removeChannel(Channel* channel) = 0;

    virtual bool hasChannel(Channel* channel) const;

    static Poller* newDefaultPoller(EventLoop* loop);

    void assertInLoopThread() const
    {
        ownerLoop_->assertInLoopThread();
    }

protected:
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_;
};

}