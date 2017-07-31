//
// Created by zhangkuo on 17-7-28.
//

#ifndef WEBSERVER_SOCKET_H
#define WEBSERVER_SOCKET_H

#include <boost/noncopyable.hpp>

namespace WebServer
{

class Socket:boost::noncopyable
{
public:
    explicit Socket(int sockfd):sockfd_(sockfd) {}
    ~Socket();

    int fd() const { return sockfd_; }

    void listen();
    void accept();

private:
    int sockfd_;
};

}

#endif //WEBSERVER_SOCKET_H
