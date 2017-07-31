//
// Created by zhangkuo on 17-7-28.
//

#include "InetAddress.h"
#include <string.h>
#include "Endian.h"
#include "SocketOps.h"
using namespace WebServer;

static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

//     /* Structure describing an Internet socket address.  */
//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

//     /* Internet address. */
//     typedef uint32_t in_addr_t;
//     struct in_addr {
//         in_addr_t       s_addr;     /* address in network byte order */
//     };

//     struct sockaddr_in6 {
//         sa_family_t     sin6_family;   /* address family: AF_INET6 */
//         uint16_t        sin6_port;     /* port in network byte order */
//         uint32_t        sin6_flowinfo; /* IPv6 flow information */
//         struct in6_addr sin6_addr;     /* IPv6 address */
//         uint32_t        sin6_scope_id; /* IPv6 scope-id */
//     };

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
    if(ipv6)
    {
        memset(&addr6_, 0, sizeof(addr6_));
        addr6_.sin6_family = AF_INET6;
        in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
        addr6_.sin6_addr = ip;
        addr6_.sin6_port = sockets::hostToNetwork16(port);
    }
    else
    {
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = sockets::hostToNetwork32(loopbackOnly ? kInaddrLoopback : kInaddrAny);
        addr_.sin_port = sockets::hostToNetwork16(port);
    }

}

InetAddress::InetAddress(std::string& ip, uint16_t port, bool ipv6)
{
    if(ipv6)
    {
        memset(&addr6_, 0, sizeof(addr6_));
        sockets::fromIpPort(ip.c_str(), port, &addr6_);
    }
    else
    {
        memset(&addr_, 0, sizeof(addr_));
        sockets::fromIpPort(ip.c_str(), port, &addr_);
    }
}

std::string InetAddress::toIp() const
{

}