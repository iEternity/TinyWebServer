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

int Socket::accept(InetAddress* peerAddr)
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
    return connfd;
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

bool Socket::getTcpInfoString(char* buf, size_t len)
{
    struct tcp_info tcpi;
    bool ok = getTcpInfo(&tcpi);
    if(ok)
    {
        snprintf(buf, len, "unrecovered = %u "
                 "rto = %u ato = %u snd_mss = %u rcv_mss = %u "
                 "lost = %u retrans = %u rtt = %u rttvar = %u "
                 "sshthresh = %u cwnd = %u total_retrans = %u",
                  tcpi.tcpi_retransmits,    //超时重传的次数
                  tcpi.tcpi_rto,            //超时时间（微秒）
                  tcpi.tcpi_ato,            //延时确认的估值
                  tcpi.tcpi_snd_mss,        //本端每个分节的最大字节数
                  tcpi.tcpi_rcv_mss,        //对端每个分节的最大字节数
                  tcpi.tcpi_lost,           //丢失未恢复的数据段数
                  tcpi.tcpi_retrans,        //重传且未确认的数据段数
                  tcpi.tcpi_rtt,            //平滑的rtt
                  tcpi.tcpi_rttvar,         //
                  tcpi.tcpi_snd_ssthresh,   //慢启动阈值
                  tcpi.tcpi_snd_cwnd,       //拥塞窗口
                  tcpi.tcpi_total_retrans); //本连接的总重传个数
    }

    return ok;
}