//
// Created by zhangkuo on 17-8-6.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include <boost/noncopyable.hpp>
#include "Channel.h"
#include "boost/scoped_ptr.hpp"
#include "Poller.h"
#include "/base/Timestamp.h"

namespace WebServer
{

class EventLoop : boost::noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
private:
    void handleRead();
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    bool looping_;
    bool quit_;
    bool eventHandling_;

    int wakeupFd_;
    boost::scoped_ptr<Channel> wakeupChannel_;
    boost::scoped_ptr<Poller> poller_;

    ChannelList activeChannels_;
    Timestamp pollReturnTime_;
    Channel* currentActiveChannel_;
};

}

#endif //WEBSERVER_EVENTLOOP_H
