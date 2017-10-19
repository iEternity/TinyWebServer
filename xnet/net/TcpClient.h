//
// Created by zhangkuo on 17-9-24.
//

#ifndef WEBSERVER_TCPCLIENT_H
#define WEBSERVER_TCPCLIENT_H
#include "InetAddress.h"
#include "TcpConnection.h"
#include "../base/Mutex.h"
#include "../base/WeakCallback.h"

namespace xnet
{

class EventLoop;
class Connector;
using ConnectorPtr = std::shared_ptr<Connector>;

class TcpClient
{
public:
    using ConnectorPtr = std::shared_ptr<Connector>;

    TcpClient(const TcpClient&) = delete;
    TcpClient& operator= (const TcpClient&) = delete;
    TcpClient(EventLoop* loop, const InetAddress& servAddr, const std::string& nameArg);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    void enableRetry() { retry_ == true; }
    bool retry() const { return retry_; }
private:
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop*              loop_;
    ConnectorPtr            connector_;
    InetAddress             servAddr_;
    std::string             name_;
    ConnectionCallback      connectionCallback_;
    MessageCallback         messageCallback_;
    WriteCompleteCallback   writeCompleteCallback_;
    int                     nextConnId_;
    mutable MutexLock       mutex_;
    bool                    retry_;
    TcpConnectionPtr        connection_;
    bool                    connect_;

};

}

#endif //WEBSERVER_TCPCLIENT_H
