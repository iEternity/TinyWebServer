//
// Created by zhangkuo on 17-7-28.
//

#ifndef WEBSERVER_INETADDRESS_H
#define WEBSERVER_INETADDRESS_H

#include <arpa/inet.h>
#include <string>
#include "SocketOps.h"

namespace xnet
{

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);
    InetAddress(std::string& ip, uint16_t port, bool ipv6 = false);

    explicit InetAddress(const sockaddr_in& addr):addr_(addr) { }
    explicit InetAddress(const sockaddr_in6& addr):addr6_(addr) { }

    sa_family_t family() const {return addr_.sin_family; }
    std::string toIp()const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr* getSockAddr()const
    {
        return sockets::sockaddr_cast(&addr6_);
    }

    void setSockAddrInet6(const sockaddr_in6& addr)
    {
        addr6_ = addr;
    }

    uint16_t portNetEndian()const
    {
        return addr_.sin_port;
    }
private:
    union
    {
        sockaddr_in addr_;
        sockaddr_in6 addr6_;
    };
};

}

#endif //WEBSERVER_INETADDRESS_H
