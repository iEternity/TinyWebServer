//
// Created by zhangkuo on 17-9-16.
//

#ifndef XNET_ACCEPTOR_H
#define XNET_ACCEPTOR_H

#include <xnet/base/noncopyable.h>
#include <xnet/net/Socket.h>
#include <xnet/net/Channel.h>

namespace xnet
{
class EventLoop;
class InetAddress;

class Acceptor : noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int fd, InetAddress)>;

public:
    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        newConnectionCallback_ = cb;
    }

    void listen();
    bool listening() const { return listening_; }

private:
    void handleRead();

private:
    EventLoop*  loop_;
    Socket      acceptSocket_;
    Channel     acceptChannel_;
    bool        listening_;
    int         idleFd_;

    NewConnectionCallback newConnectionCallback_;
};

}

#endif //XNET_ACCEPTOR_H
