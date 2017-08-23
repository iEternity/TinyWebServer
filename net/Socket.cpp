//
// Created by zhangkuo on 17-8-1.
//
#include "Socket.h"
#include "SocketOps.h"
#include <string.h>
#include <iostream>
using namespace WebServer;

Socket::~Socket()
{
    sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& localAddr)
{
    sockets::bindOrDie(sockfd_, localAddr.getSockAddr());
}

void Socket::listen()
{
    sockets::listenOrDie(sockfd_);
}

void Socket::accept(InetAddress* peerAddr)
{
    sockaddr_in6 addr;
    memset(&addr, 0, sizeof addr);

    int connfd = sockets::accept(sockfd_, &addr);
    if(connfd > 0)
    {
        peerAddr->setSockAddrInet6(addr);
    }
    else
    {
        std::cerr << "Socket::accept() error!" << std::endl;
        exit(1);
    }

}

void Socket::shutdownWrite()
{
    sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
    sockets::setTcpNoDelay(sockfd_, on);
}

void Socket::setReuseAddr(bool on)
{
    sockets::setReuseAddr(sockfd_, on);
}

void Socket::setReusePort(bool on)
{
    sockets::setReusePort(sockfd_, on);
}

void Socket::setKeepAlive(bool on)
{
    sockets::setKeepAlive(sockfd_, on);
}

bool Socket::getTcpInfo(tcp_info* tcpInfo)
{
    return sockets::getTcpInfo(sockfd_,tcpInfo);
}

