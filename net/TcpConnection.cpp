//
// Created by zhangkuo on 17-9-17.
//
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "../base/WeakCallback.h"

using namespace WebServer;
using namespace std::placeholders;

void WebServer::defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp receiveTime)
{
    buffer->retrieveAll();
}

void WebServer::defaultConnectionCallback(const TcpConnectionPtr& conn)
{

}

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
    : loop_(loop),
      name_(name),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop, sockfd)),
      localAddr_(localAddr),
      peerAddr_(peerAddr),
      highWaterMark_(64*1024*1024),
      reading_(true)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));

    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    assert(state_ == StateE::kDisconnected);
}

bool TcpConnection::getTcpInfo(struct tcp_info* tcpi) const
{
    return socket_->getTcpInfo(tcpi);
}

std::string TcpConnection::getTcpInfoString() const
{
    char buf[1024];
    buf[0] = '\0';
    socket_->getTcpInfoString(buf, sizeof(buf));
    return buf;
}

void TcpConnection::send(const void* data, size_t len)
{
    send(StringPiece(static_cast<const char*>(data), len));
}

void TcpConnection::send(const StringPiece& message)
{
    if(state_ == StateE::kConnected)
    {
        if(loop_->isInLoopThread())
        {
            sendInLoop(message);
        }
        else
        {
            loop_->runInLoop(std::bind(static_cast<void(TcpConnection::*)(StringPiece&&)>(&TcpConnection::sendInLoop),
                                       shared_from_this(),
                                       std::move(message.asString())));
        }
    }
}

void TcpConnection::send(Buffer* buf)
{
    if(state_ == StateE::kConnected)
    {
        if(loop_->isInLoopThread())
        {
            sendInLoop(buf->peek(), buf->readableBytes());
        }
        else
        {
            loop_->runInLoop(std::bind(static_cast<void(TcpConnection::*)(const StringPiece&)>(&TcpConnection::sendInLoop),
                                       shared_from_this(),
                                       std::move(buf->retrieveAllAsString())));
        }
    }
}

void TcpConnection::sendInLoop(const StringPiece& message)
{
    sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(StringPiece&& message)
{
    StringPiece s = std::move(message);
    sendInLoop(s.data(), s.size());
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
    ssize_t nWrote = 0;
    ssize_t remaining = len;
    bool faultError = false;

    if(state_ == StateE::kDisconnected)
    {
        return;
    }

    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
        nWrote = sockets::write(channel_->fd(), data, len);
        if(nWrote >= 0)
        {
            remaining = len - nWrote;
            if(remaining == 0 && writeCompleteCallback_)
            {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
        }
        else
        {
            nWrote = 0;
            if(errno != EWOULDBLOCK)    //send buffer is full or receive buffer is empty
            {
                /*
                 * EPIPE: The socket is shut down for writing
                 * ECONNRESET: connection reset by peer
                 */
                if(errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
        }
    }

    if(!faultError && remaining > 0)
    {
        size_t oldLen = outputBuffer_.readableBytes();
        if(oldLen + remaining > highWaterMark_ &&
           oldLen < highWaterMark_ && highWaterMarkCallback_)
        {
            loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldLen+remaining));
        }
        outputBuffer_.append(static_cast<const char*>(data) + nWrote, remaining);
        if(!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::shutdown()
{
    if(state_ == StateE::kConnected)
    {
        setState(StateE::kDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, shared_from_this()));
    }
}

void TcpConnection::shutdownInLoop()
{
    if(!channel_->isWriting())
    {
        socket_->shutdownWrite();
    }
}

void TcpConnection::forceClose()
{
    if(state_ == StateE::kConnected || state_ == StateE::kConnecting)
    {
        setState(StateE::kDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void TcpConnection::forceCloseWithDelay(double seconds)
{
    if(state_ == StateE::kConnected || state_ == StateE::kDisconnecting)
    {
        setState(StateE::kDisconnecting);
        loop_->runAfter(seconds, makeWeakCallback(shared_from_this(), &TcpConnection::forceClose));
    }
}

void TcpConnection::forceCloseInLoop()
{
    if(state_ == StateE::kDisconnected || state_ == StateE::kDisconnecting)
    {
        handleClose();
    }
}

const char* TcpConnection::stateToString() const
{
    switch(state_)
    {
        case StateE::kConnected :
            return "kConnected";
        case StateE::kDisconnected :
            return "kDisconnected";
        case StateE::kConnecting :
            return "kConnecting";
        case StateE::kDisconnecting :
            return "kDisconnecting";
        default:
            return "unknown state";
    }
}

void TcpConnection::setTcpNoDelay(bool on)
{
    socket_->setTcpNoDelay(on);
}

void TcpConnection::startRead()
{
    loop_->runInLoop(std::bind(&TcpConnection::startReadInLoop, shared_from_this()));
}

void TcpConnection::startReadInLoop()
{
    if(!reading_ || !channel_->isReading())
    {
        reading_ = true;
        channel_->enableReading();
    }
}

void TcpConnection::stopRead()
{
    loop_->runInLoop(std::bind(&TcpConnection::stopReadInLoop, shared_from_this()));
}

void TcpConnection::stopReadInLoop()
{
    if(reading_ || channel_->isReading())
    {
        reading_ = false;
        channel_->disableReading();
    }
}

void TcpConnection::connectEstablished()
{
    setState(StateE::kConnected);
    channel_->enableReading();
    channel_->tie(shared_from_this());

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    if(state_ == StateE::kConnected)
    {
        setState(StateE::kDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }

    channel_->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if(n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    }
    else if(n == 0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    if(channel_->isWriting())
    {
        ssize_t n = sockets::write(channel_->fd(),
                                   static_cast<const void*>(inputBuffer_.peek()),
                                   inputBuffer_.readableBytes());
        if(n > 0)
        {
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0)
            {
                channel_->disableWriting();
                if(writeCompleteCallback_)
                {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if(state_ == StateE::kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {

        }
    }
}

void TcpConnection::handleError()
{

}

void TcpConnection::handleClose()
{
    setState(StateE::kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);
    closeCallback_(guardThis);
}
