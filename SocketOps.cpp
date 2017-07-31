//
// Created by zhangkuo on 17-7-30.
//
#include "SocketOps.h"
#include "Endian.h"
#include <iostream>
#include <boost/implicit_cast.hpp>

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