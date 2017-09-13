//
// Created by zhangkuo on 17-9-12.
//

#ifndef WEBSERVER_TCPSERVER_H
#define WEBSERVER_TCPSERVER_H
#include <boost/noncopyable.hpp>
#include <functional>
#include "InetAddress.h"

namespace WebServer
{

class EventLoop;

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

    const std::string& ipPort() const { return ipPort_; }
    const std::string& name() const { return name_; }
    EventLoop* getLoop() const {return loop_; }

    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback& cb)
    { threadInitCallback_ = cb; }
    void setThreadInitCallback(ThreadInitCallback&& cb)
    { threadInitCallback_ = std::move(cb); }



private:
    EventLoop* loop_;
    const std::string ipPort_;
    const std::string name_;

    ThreadInitCallback threadInitCallback_;
};

}

#endif //WEBSERVER_TCPSERVER_H
