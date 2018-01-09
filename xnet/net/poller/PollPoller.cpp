//
// Created by zhangkuo on 17-8-9.
//
#include <xnet/net/poller/PollPoller.h>
#include <xnet/base/Logging.h>
using namespace xnet;

PollPoller::PollPoller(EventLoop* loop)
    : Poller(loop)
{

}

PollPoller::~PollPoller()
{

}

Timestamp PollPoller::poll(int timeoutMS, ChannelList* activeChannels)
{
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMS);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if(numEvents > 0)
    {
        LOG_TRACE << numEvents << "events happened";
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0)
    {
        LOG_TRACE << "nothing happened";
    }
    else
    {
        if(savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_SYSERR << "PollPoller::poll";
        }
    }

    return now;
}

void PollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for(auto it = pollfds_.begin(); it != pollfds_.end() && numEvents >0; it++)
    {
        if(it->revents > 0)
        {
            numEvents --;
            auto ch = channels_.find(it->fd);
            Channel* channel = ch->second;
            channel->setRevents(it->revents);
            activeChannels->push_back(channel);
        }
    }
}

void PollPoller::updateChannel(Channel* channel)
{
    Poller::assertInLoopThread();

    LOG_TRACE << "fd = " << channel->fd() << " events = {" << channel->eventsToString() <<"}";

    if(channel->index() < 0)
    {
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        pollfds_.push_back(pfd);

        int idx = static_cast<int>(pollfds_.size()) - 1;
        channel->setIndex(idx);
        channels_[pfd.fd] = channel;
    }
    else
    {
        int idx = channel->index();
        struct pollfd& pfd = pollfds_[idx];
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        if(channel->isNoneEvent())
        {
            /* ignore this pollfd */
            pfd.fd = -channel->fd() -1;
        }
    }
}

void PollPoller::removeChannel(Channel* channel)
{
    Poller::assertInLoopThread();

    LOG_TRACE << "fd = " << channel->fd();

    int idx = channel->index();
    channels_.erase(channel->fd());
    if(idx == static_cast<int>(pollfds_.size()-1))
    {
        pollfds_.pop_back();
    }
    else
    {
        int channelAtEnd = pollfds_.back().fd;
        std::iter_swap(pollfds_.begin() + idx, pollfds_.end()-1);
        if(channelAtEnd < 0)
        {
            channelAtEnd = -channelAtEnd -1;
        }
        channels_[channelAtEnd]->setIndex(idx);
        pollfds_.pop_back();
    }
}