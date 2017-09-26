//
// Created by zhangkuo on 17-9-24.
//
#include "TcpClient.h"
#include "EventLoop.h"
#include "Connector.h"

using namespace WebServer;
using namespace std;
using namespace std::placeholders;

namespace WebServer
{
namespace detail
{
 void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
 {
     loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
 }

 void removeConnector(const ConnectorPtr& connector)
 {

 }
}
}

TcpClient::TcpClient(EventLoop* loop, const InetAddress& servAddr, const std::string& nameArg):
    loop_(loop),
    connector_(new Connector(loop, servAddr)),
    servAddr_(servAddr),
    name_(nameArg),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback),
    nextConnId_(1),
    retry_(false)
{
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, _1));
}

TcpClient::~TcpClient()
{
    TcpConnectionPtr conn;
    bool unique = false;
    {
        MutexLockGuard lock(mutex_);
        unique = connection_.use_count() == 1;
        conn = connection_;
    }

    if(conn)
    {
        CloseCallback cb = std::bind(&detail::removeConnection, loop_, _1);
        loop_->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if(unique)
        {
            conn->forceClose();
        }
    }
    else
    {
        connector_->stop();
        loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
    }
}

void TcpClient::connect()
{
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect()
{
    connect_ = false;
    {
        MutexLockGuard lock(mutex_);
        if(connection_)
        {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop()
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof(buf), ":%s#%d", peerAddr.toIpPort().data(), nextConnId_);
    ++nextConnId_;
    string nameConn = name_ + buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(loop_, nameConn, sockfd,
    localAddr, peerAddr);

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, conn));

    {
        MutexLockGuard lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    {
        MutexLockGuard lock(mutex_);
        connection_.reset();
    }
    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if(retry_ && connect_)
    {
        connector_->restart();
    }
}

