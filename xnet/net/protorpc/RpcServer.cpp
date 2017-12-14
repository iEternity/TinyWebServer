//
// Created by zhangkuo on 17-12-9.
//
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <xnet/net/protorpc/RpcServer.h>
#include <xnet/base/Logging.h>
#include <xnet/net/TcpConnection.h>
#include <xnet/net/protorpc/RpcChannel.h>

using namespace std;
using namespace std::placeholders;
using namespace google;
using namespace xnet;

RpcServer::RpcServer(EventLoop* loop, const InetAddress& listenAddr):
    server_(loop, listenAddr, "RpcServer")
{
    server_.setConnectionCallback(std::bind(&RpcServer::onConnection, this, _1));
}

void RpcServer::registerService(protobuf::Service* service)
{
    const auto desc = service->GetDescriptor();
    services_[desc->full_name()] = service;
}

void RpcServer::start()
{
    server_.start();
}

void RpcServer::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO << "RpcServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected()? "UP" : "DOWN");

    if(conn->connected())
    {
        RpcChannelPtr channel(new RpcChannel(conn));
        channel->setServices(&services_);
        conn->setMessageCallback(std::bind(&RpcChannel::onMessage, channel.get(), _1, _2, _3));
        conn->setContext(channel);
    }
    else
    {
        conn->setContext(RpcChannelPtr());
    }
}