//
// Created by zhangkuo on 17-9-14.
//

#include "TcpServer.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"

using namespace WebServer;

TcpServer::TcpServer(EventLoop* loop,
                     const InetAddress& listenAddr,
                     const std::string& nameArg,
                     Option option):
        loop_(loop),
        ipPort_(listenAddr.toIpPort()),
        name_(nameArg),
        acceptor_(new Acceptor(loop_, listenAddr, option == kReusePort)),
        threadPool_(new EventLoopThreadPool(loop, nameArg))

{

}
