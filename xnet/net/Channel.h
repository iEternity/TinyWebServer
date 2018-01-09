//
// Created by zhangkuo on 17-8-7.
//

#ifndef XNET_CHANNEL_H
#define XNET_CHANNEL_H

#include <functional>
#include <memory>
#include <xnet/net/EventLoop.h>
#include <xnet/base/noncopyable.h>
#include <xnet/base/Timestamp.h>

namespace xnet
{

class EventLoop;

class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

public:
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

    int     events() const          { return events_; }
    void    setRevents(int events)  { revents_ = events; }

    void    setIndex(int idx)   { index_ = idx; }
    int     index() const       { return index_; }

    void enableReading()    { events_ |= kReadEvent; update(); }
    void disableReading()   { events_ &= ~kReadEvent; update(); }
    void enableWriting()    { events_ |= kWriteEvent; update(); }
    void disableWriting()   { events_ &= ~kWriteEvent; update(); }
    void disableAll()       { events_ = kNoneEvent; update(); }

    bool isReading() const      { return static_cast<bool>(events_ & kReadEvent); }
    bool isWriting() const      { return static_cast<bool>(events_ & kWriteEvent); }
    bool isNoneEvent() const    { return events_ == kNoneEvent; }

    void remove();

    // for debug
    std::string eventsToString() const;
    std::string reventsToString() const;

private:
    void update();

    void handleEventWithGuard(Timestamp receiveTime);

    std::string eventsToString(int fd, int event) const;

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop*  loop_;
    const int   fd_;
    int         events_;    //关心的事件
    int         revents_;   //发生的事件
    int         index_;

    ReadEventCallback   readCallback_;
    EventCallback       writeCallback_;
    EventCallback       closeCallback_;
    EventCallback       errorCallback_;

    bool tied_;
    std::weak_ptr<void> tie_;
};

}
#endif //XNET_CHANNEL_H
