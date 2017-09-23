//
// Created by zhangkuo on 17-7-28.
//

#ifndef WEBSERVER_SOCKET_H
#define WEBSERVER_SOCKET_H

#include <boost/noncopyable.hpp>
#include "InetAddress.h"
#include <netinet/tcp.h>

namespace WebServer
{

class Socket:boost::noncopyable
{
public:
    explicit Socket(int sockfd):sockfd_(sockfd) {}
    ~Socket();

    int fd() const { return sockfd_; }

    void bindAddress(const InetAddress& localAddr);
    void listen();

    int accept(InetAddress* peerAddr);
    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

    bool getTcpInfo(tcp_info* tcpInfo);
    bool getTcpInfoString(char* buf, size_t len);

private:
    const int sockfd_;
};

}

#endif //WEBSERVER_SOCKET_H
