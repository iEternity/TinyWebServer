#include <xnet/net/http/HttpServer.h>

using namespace std;
using namespace xnet;

HttpServer::HttpServer(EventLoop* loop,
                       const InetAddress& addr,
                       const string& name,
                       TcpServer::Option option):
    server_(loop, addr, name, option)
{
    
}