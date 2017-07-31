//
// Created by zhangkuo on 17-7-30.
// This is internal header file ,you should not include this

#ifndef WEBSERVER_SOCKETOPS_H
#define WEBSERVER_SOCKETOPS_H

#include <arpa/inet.h>

namespace WebServer
{
namespace sockets
{

void fromIpPort(const char* ip, uint16_t port, sockaddr_in* addr);
void fromIpPort(const char* ip, uint16_t port, sockaddr_in6* addr);

void toIp(char* buf, size_t size, const sockaddr* addr);
void toIpPort(char* buf, size_t size, const sockaddr* addr);

const sockaddr* sockaddr_cast(const sockaddr_in* addr);
const sockaddr* sockaddr_cast(const sockaddr_in6* addr);
const sockaddr_in* sockaddr_in_cast(const sockaddr* addr);
const sockaddr_in6* sockaddr_in6_cast(const sockaddr* addr);

}
}

#endif //WEBSERVER_SOCKETOPS_H
