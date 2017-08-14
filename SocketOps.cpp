//
// Created by zhangkuo on 17-7-30.
//
#include "SocketOps.h"
#include "Endian.h"
#include <iostream>
#include <boost/implicit_cast.hpp>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>

using namespace WebServer;
using namespace boost;

const sockaddr* sockets::sockaddr_cast(const sockaddr_in* addr)
{
    return static_cast<const sockaddr*>(implicit_cast<const void*>(addr));
}

const sockaddr* sockets::sockaddr_cast(const sockaddr_in6* addr)
{
    return static_cast<const sockaddr*>(implicit_cast<const void*>(addr));
}

sockaddr* sockets::sockaddr_cast(sockaddr_in* addr)
{
    return static_cast<sockaddr*>(implicit_cast<void*>(addr));
}

sockaddr* sockets::sockaddr_cast(sockaddr_in6* addr)
{
    return static_cast<sockaddr*>(implicit_cast<void*>(addr));
}

const sockaddr_in* sockets::sockaddr_in_cast(const sockaddr* addr)
{
    return static_cast<const sockaddr_in*>(implicit_cast<const void*>(addr));
}

const sockaddr_in6* sockets::sockaddr_in6_cast(const sockaddr* addr)
{
    return static_cast<const sockaddr_in6*>(implicit_cast<const void*>(addr));
}

void sockets::fromIpPort(const char* ip, uint16_t port, sockaddr_in* addr)
{
    addr->sin_family    = AF_INET;
    addr->sin_port      = hostToNetwork16(port);
    if(inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    {
        std::cerr<<"sockets::fromIpPort error!"<<std::endl;
        exit(1);
    }
}

void sockets::fromIpPort(const char* ip, uint16_t port, sockaddr_in6* addr)
{
    addr->sin6_family   = AF_INET6;
    addr->sin6_port     = hostToNetwork16(port);
    if(inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
    {
        std::cerr<<"sockets::fromIpPort error!"<<std::endl;
        exit(1);
    }
}

void sockets::toIp(char* buf, size_t size, const sockaddr* addr)
{
    if(addr->sa_family == AF_INET)
    {
        assert(size >= INET_ADDRSTRLEN);
        const sockaddr_in* addr4 = sockaddr_in_cast(addr);
        inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
    }
    else if(addr->sa_family == AF_INET6)
    {
        assert(size >= INET6_ADDRSTRLEN);
        const sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
        inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
    }
}

void sockets::toIpPort(char* buf, size_t size, const sockaddr* addr)
{
    toIp(buf, size, addr);
    size_t end = strlen(buf);
    uint16_t port;

    if(addr->sa_family == AF_INET)
    {
        const sockaddr_in* addr4 = sockaddr_in_cast(addr);
        port = networkToHost16(addr4->sin_port);
    }
    else if(addr->sa_family == AF_INET6)
    {
        const sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
        port = networkToHost16(addr6->sin6_port);
    }

    assert(size > end);
    snprintf(buf+end, size-end, ":%u", port);
}



void sockets::close(int sockfd)
{
    if(::close(sockfd) < 0)
    {
        std::cerr << "sockets::close() error!" << std::endl;
        exit(1);
    }
}

void sockets::bindOrDie(int sockfd, const sockaddr* addr)
{
    int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof sockaddr_in6));
    if(ret < 0)
    {
        std::cerr << "sockets::bindOrDie() error!" << std::endl;
        exit(1);
    }
}

void sockets::listenOrDie(int sockfd)
{
    int ret = ::listen(sockfd, SOMAXCONN);
    if(ret < 0)
    {
        std::cerr << "sockets::listenOrDie() error!" << std::endl;
        exit(1);
    }
}

int sockets::accept(int sockfd, sockaddr_in6* addr)
{
    socklen_t addrLen = static_cast<socklen_t>(sizeof(*addr));
    int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrLen);
    setNonBlockAndCloseOnExec(connfd);

    return connfd;
}

void sockets::shutdownWrite(int sockfd)
{
    if(::shutdown(sockfd, SHUT_WR) < 0)
    {
        std::cerr << "sockets::shutdownWrite() error!" << std::endl;
        exit(1);
    }
}

void sockets::setNonBlockAndCloseOnExec(int sockfd)
{
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    ::fcntl(sockfd, F_SETFL, flags);

    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ::fcntl(sockfd, F_SETFD, flags);
}

void sockets::setTcpNoDelay(int sockfd, bool on)
{
    int optVal = on ? 1 : 0;
    ::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optVal, static_cast<socklen_t>(sizeof optVal));
}

void sockets::setReuseAddr(int sockfd, bool on)
{
    int optVal = on ? 1 : 0;
    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optVal, static_cast<socklen_t>(sizeof optVal));
}

void sockets::setReusePort(int sockfd, bool on)
{
    int optVal = on ? 1 : 0;
    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optVal, static_cast<socklen_t>(sizeof optVal));
}

void sockets::setKeepAlive(int sockfd, bool on)
{
    int optVal = on ? 1 : 0;
    ::setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optVal, static_cast<socklen_t>(sizeof optVal));
}

bool sockets::getTcpInfo(int sockfd, tcp_info* tcpInfo)
{
    socklen_t len = static_cast<socklen_t>(tcpInfo);
    return ::getsockopt(sockfd, SOL_TCP, TCP_INFO, tcpInfo, &len) == 0;
}