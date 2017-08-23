//
// Created by zhangkuo on 17-8-7.
//

#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H

#include "EventLoop.h"
#include <boost/noncopyable.hpp>
#include <functional>
#include <memory>
#include "../base/Timestamp.h"

namespace WebServer
{

class EventLoop;

class Channel : boost::noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);
    void setReadCallback(const ReadEventCallback& cb) { readCallback_ = cb; }
    void setReadCallback(ReadEventCallback&& cb) { readCallback_ = std::move(cb); }

    void setWriteCallback(const EventCallback& cb) { writeCallback_ = cb; }
    void setWriteCallback(EventCallback&& cb) { writeCallback_ = std::move(cb); }

    void setCloseCallback(const EventCallback& cb) { closeCallback_ = cb; }
    void setCloseCallback(const EventCallback&& cb) { closeCallback_ = std::move(cb); }

    void setErrorCallback(const EventCallback& cb) { errorCallback_ = cb; }
    void setErrorCallback(EventCallback&& cb) { errorCallback_ = std::move(cb); }

    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }

    int events() const { return events_; }
    void setRevents(int events) { revents_ = events; }
    bool isNoneEvent() const { events_ == kNoneEvent; }

    void setIndex(int idx) { index_ = idx; }
    int index() const { return index_; }

    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriteing() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }
    bool isReading() const { return events_ & kReadEvent; }
    bool isWriteing() const { return events_ & kWriteEvent; }

    void remove();

private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    bool addedToLoop_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

}
#endif //WEBSERVER_CHANNEL_H
