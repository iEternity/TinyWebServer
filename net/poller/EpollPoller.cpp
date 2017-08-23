//
// Created by zhangkuo on 17-8-13.
//
#include "EpollPoller.h"
#include <string.h>
#include <boost/implicit_cast.hpp>
using namespace WebServer;
using namespace boost;

namespace
{
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;
}

EpollPoller::EpollPoller(EventLoop* loop)
        : Poller(loop),
          epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
          events_(kInitEventListSize)
{
    if (epollfd_ < 0)
    {
        //LOG_SYSFATAL
    }
}

EpollPoller::~EpollPoller()
{
    ::close(epollfd_);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::epoll_wait(epollfd_,
                                 &*events_.begin(),
                                 static_cast<int>(events_.size()),
                                 timeoutMs);
    Timestamp now(Timestamp::now());

    if(numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        if(implicit_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }

    return now;
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for(int i=0; i<numEvents; i++)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::updateChannel(Channel* channel)
{
    int idx = channel->index();
    if(idx == kNew)
    {
        channel->setIndex(kAdded);
        channels_[channel->fd()] = channel;

        update(EPOLL_CTL_ADD, channel);
    }
    else if (idx == kDeleted)
    {
        channel->setIndex(kAdded);

        update(EPOLL_CTL_ADD, channel);
    }
    else    // idx = kAdded
    {
        int fd = channel->fd();
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel* channel)
{
    channels_.erase(channel->fd());
    if(channel->index() == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void EpollPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;

    ::epoll_ctl(epollfd_, operation, channel->fd(), &event);
}