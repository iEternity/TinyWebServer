//
// Created by zhangkuo on 17-8-7.
//
#include <sstream>
#include <poll.h>
#include <xnet/net/Channel.h>
#include <xnet/base/Logging.h>
using namespace xnet;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false)
{

}

Channel::~Channel()
{

}

void Channel::handleEvent(Timestamp receiveTime)
{
    if(tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if(guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_TRACE << reventsToString();

    if((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        LOG_WARN << "fd = " << fd_ << "Channel::handleEvent() POLLHUP";
        if(closeCallback_) closeCallback_();
    }

    if(revents_ & (POLLERR | POLLNVAL))
    {
        if(revents_ & POLLNVAL)
        {
            LOG_WARN << "fd = " << fd_ << "Channel::handleEvent() POLLNVAL";
        }

        if(errorCallback_) errorCallback_();
    }

    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if(readCallback_) readCallback_(receiveTime);
    }

    if(revents_ & POLLOUT)
    {
        if(writeCallback_) writeCallback_();
    }
}


void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::remove()
{
    loop_->removeChannel(this);
}

std::string Channel::eventsToString() const
{
    return eventsToString(fd_, events_);
}

std::string Channel::reventsToString() const
{
    return eventsToString(fd_, revents_);
}

std::string Channel::eventsToString(int fd, int event) const
{
    std::ostringstream oss;
    oss << fd << ": ";

    if(event & POLLIN) oss << "IN ";

    if(event & POLLOUT) oss << "OUT ";

    if(event & POLLHUP) oss << "HUP ";

    if(event & POLLRDHUP) oss << "RDHUP ";

    if(event & POLLPRI) oss << "PRI ";

    if(event & POLLERR) oss << "ERR ";

    if(event & POLLNVAL) oss << "NVAL ";

    return oss.str();
}