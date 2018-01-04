#pragma once
#include <xnet/base/noncopyable.h>
#include <xnet/net/TcpServer.h>
#include <xnet/net/http/HttpRequest.h>
#include <xnet/net/http/HttpResponse.h>

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

    void setHttpCallback(const HttpCallback& cb) { httpCallback_ = cb; }
    void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }
    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);
    void onRequest(const TcpConnectionPtr&, const HttpRequest& request);

private:
    TcpServer       server_;
    HttpCallback    httpCallback_;
};

}