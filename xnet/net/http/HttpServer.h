#pragma once
#include <xnet/base/noncopyable.h>
#include <xnet/net/TcpServer.h>

namespace xnet
{

class HttpServer : noncopyable
{
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;
public:
    HttpServer(EventLoop* loop, 
               const InetAddress& addr, 
               const string& name,
               TcpServer::Option option = TcpServer::Option::kNoReusePort);

    ~HttpServer(){};

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);

private:
    TcpServer       server_;
    HttpCallback    httpCallback_;
};

}