//
// Created by zhangkuo on 17-12-9.
//

#ifndef XNET_RPCSERVER_H
#define XNET_RPCSERVER_H
#include <xnet/net/TcpServer.h>
#include <map>

namespace google
{
namespace protobuf
{
    class Service;
}
}

namespace xnet
{

class RpcServer
{
public:
    RpcServer(EventLoop* loop, const InetAddress& listenAddr);

    void setThreadNum(int numThreads)
    {
        server_.setThreadNum(numThreads);
    }

    void registerService(google::protobuf::Service* );
    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);

private:
    TcpServer server_;
    std::map<std::string, google::protobuf::Service*> services_;
};

}

#endif //XNET_RPCSERVER_H
