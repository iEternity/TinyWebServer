//
// Created by zhangkuo on 17-9-24.
//
#include "Connector.h"
#include "EventLoop.h"
#include "Channel.h"
using namespace WebServer;

const int Connector::kMaxRetryDelayMs;
const int Connector::kInitRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& servAddr):
        loop_(loop),
        servAddr_(servAddr),
        connect_(false),
        state_(States::kDisconnected),
        retryDelayMs_(kInitRetryDelayMs)
{

}

Connector::~Connector()
{

}

void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop, shared_from_this()));
}

void Connector::startInLoop()
{
    if(connect_)
    {
        connect();
    }
}

void Connector::stop()
{
    connect_ = false;
    loop_->queueInLoop(std::bind(&Connector::stopInLoop, shared_from_this()));
}

void Connector::stopInLoop()
{
    if(state_ == States::kConnecting)
    {
        setState(States::kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::restart()
{
    connect_        = true;
    retryDelayMs_   = kInitRetryDelayMs;
    setState(States::kDisconnected);
    startInLoop();
}

void Connector::connect()
{
    int sockfd      = sockets::createNonblockingOrDie(servAddr_.family());
    int ret         = sockets::connect(sockfd, servAddr_.getSockAddr());
    int savedErrno  = ret == 0 ? 0 : errno;

    switch(savedErrno)
    {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            sockets::close(sockfd);
            break;
        default:
            sockets::close(sockfd);
            break;
    }
}

void Connector::connecting(int sockfd)
{
    setState(States::kConnecting);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, shared_from_this()));
    channel_->setErrorCallback(std::bind(&Connector::handleError, shared_from_this()));
    channel_->enableWriting();
}

void Connector::retry(int sockfd)
{
    sockets::close(sockfd);
    setState(States::kDisconnected);
    if(connect_)
    {
        loop_->runAfter(retryDelayMs_/1000.0, std::bind(&Connector::startInLoop, shared_from_this()));
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
    }
}

int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    // can't reset channel_ here, because we are inside Channel::handleEvent
    loop_->queueInLoop(std::bind(&Connector::resetChannel, shared_from_this()));

    return sockfd;
}

void Connector::resetChannel()
{
    channel_.reset();
}

void Connector::handleWrite()
{
    if(state_ == States::kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if(err)
        {
            retry(sockfd);
        }
        else if(sockets::isSelfConnect(sockfd))
        {
            retry(sockfd);
        }
        else
        {
            setState(States::kConnected);
            if(connect_)
            {
                newConnectionCallback_(sockfd);
            }
            else
            {
                sockets::close(sockfd);
            }
        }
    }
}

void Connector::handleError()
{
    if(state_ == States::kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err    = sockets::getSocketError(sockfd);
        retry(sockfd);
    }
}