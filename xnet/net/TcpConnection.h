//
// Created by zhangkuo on 17-9-17.
//

#ifndef XNET_TCPCONNECTION_H
#define XNET_TCPCONNECTION_H

#include <boost/noncopyable.hpp>
#include <memory>
#include <boost/any.hpp>
#include "InetAddress.h"
#include "../base/Timestamp.h"
#include "xnet/base/StringPiece.h"
#include "Buffer.h"
#include "Callbacks.h"

namespace xnet
{

class EventLoop;
class Socket;
class Channel;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    enum class StateE
    {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };
public:
    TcpConnection(const TcpConnection&) = delete;
    TcpConnection& operator= (const TcpConnection&) = delete;
    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop*          getLoop() const         { return loop_; }
    const std::string&  name() const            { return name_; }
    bool                connected() const       { return state_ == StateE::kConnected; }
    bool                disconnected() const    { return state_ == StateE::kDisconnected; }
    const InetAddress&  localAddress() const    { return localAddr_; }
    const InetAddress&  peerAddress() const     { return peerAddr_; }

    bool                getTcpInfo(struct tcp_info* tcpi) const;
    std::string         getTcpInfoString() const;

    void send(const void* data, size_t len);
    void send(const StringPiece& message);
    void send(Buffer* buffer);
    void shutdown();
    void forceClose();
    void forceCloseWithDelay(double seconds);
    void setTcpNoDelay(bool on);
    void startRead();
    void stopRead();
    bool isReading() const { return reading_; }; // NOT thread safe, may race with start/stopReadInLoop

    void setContext(const boost::any& context)
    {
        context_ = context;
    }

    const boost::any& getContext() const
    {
        return context_;
    }

    boost::any* getMutableContext()
    {
        return &context_;
    }

    void setConnectionCallback(const ConnectionCallback& cb)
    {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb)
    {
        messageCallback_ = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    {
        writeCompleteCallback_ = cb;
    }
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
    {
        highWaterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }
    // Internal use only
    void setCloseCallback(const CloseCallback& cb)
    {
        closeCallback_ = cb;
    }

    void connectEstablished();
    void connectDestroyed();
private:
    void        setState(StateE s) { state_ = s; }
    const char* stateToString() const ;

    void sendInLoop(const StringPiece& message);
    void sendInLoop(StringPiece&& message);
    void sendInLoop(const void* data, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();
    void startReadInLoop();
    void stopReadInLoop();

    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

private:
    EventLoop*                  loop_;
    const std::string           name_;
    std::unique_ptr<Socket>     socket_;
    std::unique_ptr<Channel>    channel_;
    InetAddress                 localAddr_;
    InetAddress                 peerAddr_;
    StateE                      state_;
    size_t                      highWaterMark_;
    bool                        reading_;

    ConnectionCallback      connectionCallback_;
    MessageCallback         messageCallback_;
    WriteCompleteCallback   writeCompleteCallback_;
    HighWaterMarkCallback   highWaterMarkCallback_;
    CloseCallback           closeCallback_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;

    boost::any context_;
};

}

#endif //XNET_TCPCONNECTION_H
