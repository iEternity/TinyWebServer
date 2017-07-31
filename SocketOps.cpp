//
// Created by zhangkuo on 17-7-30.
//
#include "SocketOps.h"
#include "Endian.h"
#include <iostream>
#include <boost/implicit_cast.hpp>
#include <assert.h>
#include <stdio.h>
#include <string.h>

using namespace WebServer;
using namespace boost;

void sockets::fromIpPort(const char* ip, uint16_t port, sockaddr_in* addr)
{
    addr->sin_family    = AF_INET;
    addr->sin_port      = hostToNetwork16(port);
    if(inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    {
        std::cerr<<"sockets::fromIpPort error!"<<std::endl;
    }
}

void sockets::fromIpPort(const char* ip, uint16_t port, sockaddr_in6* addr)
{
    addr->sin6_family   = AF_INET6;
    addr->sin6_port     = hostToNetwork16(port);
    if(inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
    {
        std::cerr<<"sockets::fromIpPort error!"<<std::endl;
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

const sockaddr* sockets::sockaddr_cast(const sockaddr_in* addr)
{
    return static_cast<const sockaddr*>(implicit_cast<const void*>(addr));
}

const sockaddr* sockets::sockaddr_cast(const sockaddr_in6* addr)
{
    return static_cast<const sockaddr*>(implicit_cast<const void*>(addr));
}

const sockaddr_in* sockets::sockaddr_in_cast(const sockaddr* addr)
{
    return static_cast<const sockaddr_in*>(implicit_cast<const void*>(addr));
}

const sockaddr_in6* sockets::sockaddr_in6_cast(const sockaddr* addr)
{
    return static_cast<const sockaddr_in6*>(implicit_cast<const void*>(addr));
}