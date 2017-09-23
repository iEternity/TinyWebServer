//
// Created by zhangkuo on 17-9-12.
//

#ifndef WEBSERVER_TCPSERVER_H
#define WEBSERVER_TCPSERVER_H
#include <boost/noncopyable.hpp>
#include <functional>
#include "InetAddress.h"
#include "Callbacks.h"
#include <boost/scoped_ptr.hpp>
#include <map>
#include <atomic>

namespace WebServer
{

class EventLoop;
class Acceptor;
class EventLoopThreadPool;

class TcpServer : boost::noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop* loop,
              const InetAddress& listenAddr,
              const std::string& nameArg,
              Option option = kNoReusePort);
    ~TcpServer();

    EventLoop*                              getLoop() const { return loop_; }
    const std::string&                      ipPort() const  { return ipPort_; }
    const std::string&                      name() const    { return name_; }
    std::shared_ptr<EventLoopThreadPool>    getThreadPool() { return threadPool_; }

    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback& cb)    { threadInitCallback_ = cb; }
    void setThreadInitCallback(ThreadInitCallback&& cb)         { threadInitCallback_ = std::move(cb); }

    void start();

private:
    using ConnectionMap = std::map<std::string, TcpConnectionPtr >;

    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    EventLoop*                              loop_;
    const std::string                       ipPort_;
    const std::string                       name_;
    std::unique_ptr<Acceptor>               acceptor_;
    std::shared_ptr<EventLoopThreadPool>    threadPool_;
    std::atomic<bool>                       started_;
    int                                     nextConnId_;

    ConnectionCallback      connectionCallback_;
    MessageCallback         messageCallback_;
    WriteCompleteCallback   writeCompleteCallback_;
    ThreadInitCallback      threadInitCallback_;

    ConnectionMap       connections_;
};

}

#endif //WEBSERVER_TCPSERVER_H
