//
// Created by zhangkuo on 18-1-1.
//
#include <iostream>
#include <xnet/net/http/HttpServer.h>
#include <xnet/net/EventLoop.h>

using namespace std;
using namespace xnet;

void onRequest(const HttpRequest& req, HttpResponse* resp)
{
    cout << "RequestLine:\n" << req.methodString() << " "
         << req.getPath() << " " << endl;

    cout << "Headers: " << endl;
    auto& headers = req.getHeaders();
    for(auto& it : headers)
    {
        cout << it.first << ": " << it.second << endl;
    }
}

int main(int argc, char* argv[])
{
    EventLoop loop;
    HttpServer server(&loop, InetAddress(8888), "HttpServer");
    server.setHttpCallback(onRequest);
    server.setThreadNum(4);
    server.start();
    loop.loop();
}