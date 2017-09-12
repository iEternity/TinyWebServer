//
// Created by zhangkuo on 17-8-7.
//
#include "Channel.h"
#include <poll.h>
using namespace WebServer;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1)
{

}

Channel::~Channel()
{

}

void Channel::handleEvent(Timestamp receiveTime)
{

}


void Channel::tie(const std::shared_ptr<void>& obj)
{

}

void Channel::update()
{
    addedToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::remove()
{
    addedToLoop_ = false;
    loop_->removeChannel(this);
}


