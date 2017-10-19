//
// Created by zhangkuo on 17-9-24.
//

#ifndef WEBSERVER_CONNECTOR_H
#define WEBSERVER_CONNECTOR_H

#include <memory>
#include "InetAddress.h"

namespace xnet
{
class EventLoop;
class Channel;

class Connector : public std::enable_shared_from_this<Connector>
{
public:
    using NewConnectionCallback = std::function<void(int sockfd)>;

    Connector(const Connector&) = delete;
    Connector& operator== (const Connector&) = delete;
    Connector(EventLoop* loop, const InetAddress& servAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb) { newConnectionCallback_ = cb; }

    void start();   //can be called in any thread
    void restart(); //must be called in loop thread
    void stop();    //can be called in any thread
private:
    enum class States
    {
        kDisconnected,
        kConnecting,
        kConnected
    };
    static const int kMaxRetryDelayMs   = 30*1000;
    static const int kInitRetryDelayMs  = 500;

    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int sockfd);
    void retry(int sockfd);
    void setState(States s) { state_ = s; }
    int  removeAndResetChannel();
    void resetChannel();
    void handleWrite();
    void handleError();

    EventLoop*                  loop_;
    InetAddress                 servAddr_;
    bool                        connect_;
    States                      state_;
    NewConnectionCallback       newConnectionCallback_;
    std::unique_ptr<Channel>    channel_;
    int                         retryDelayMs_;
};

}

#endif //WEBSERVER_CONNECTOR_H
