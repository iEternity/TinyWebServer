//
// Created by zhangkuo on 17-8-6.
//
#include "EventLoop.h"
#include <sys/eventfd.h>
#include "Poller.h"

using namespace WebServer;

namespace
{
    __thread EventLoop* t_loopInThisThread = 0;

    int createEventFd()
    {
        int evtFd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        return evtFd;
    }

    const int kPollTimeMs = 10000;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      poller_(Poller::newDefaultPoller(this)),
      wakeupFd_(createEventFd()),
      wakeupChannel_(new Channel(this, wakeupFd_))

{
    t_loopInThisThread = this;

    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    while(!quit)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        eventHandling_ = true;

        for(auto& channel:activeChannels_)
        {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }

        currentActiveChannel_ = NULL;
        eventHandling_ = false;
        doPendingFunctors();
    }

    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;

}

void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    poller_->removeChannel(channel);
}

void EventLoop::handleRead()
{

}

void EventLoop::doPendingFunctors()
{

}