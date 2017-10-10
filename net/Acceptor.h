//
// Created by zhangkuo on 17-9-16.
//

#ifndef WEBSERVER_ACCEPTOR_H
#define WEBSERVER_ACCEPTOR_H

#include <boost/noncopyable.hpp>
#include "Socket.h"
#include "Channel.h"

namespace xnet
{
class EventLoop;
class InetAddress;

class Acceptor : boost::noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int fd, InetAddress)>;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        newConnectionCallback_ = cb;
    }

    bool listening() const { return listening_; }

    void listen();
private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
    int idleFd_;
};

}

#endif //WEBSERVER_ACCEPTOR_H
