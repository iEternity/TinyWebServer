#include <xnet/net/http/HttpServer.h>
#include <xnet/net/http/HttpContext.h>

using namespace std;
using namespace xnet;
using namespace std::placeholders;

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
{
    resp->setStatusCode(HttpStatusCode::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

HttpServer::HttpServer(EventLoop* loop,
                       const InetAddress& addr,
                       const string& name,
                       TcpServer::Option option):
    server_(loop, addr, name, option),
    httpCallback_(defaultHttpCallback)
{
    server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

void HttpServer::start()
{
    LOG_WARN << "HttpServer[" << server_.name() << "] starts listening on " << server_.toIpPort();
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
{
    auto context = boost::any_cast<HttpContext>(conn->getMutableContext());
    if(!context->parse(buf, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request");
        conn->shutdown();
    }

    if(context->hasGotAll())
    {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& request)
{
    const string& connection = request.getHeader("Connection");
    bool isClose = (connection == "close") ||
        (request.getVersion() == HttpRequest::Version::kHttp10 && connection != "Keep-Alive");

    HttpResponse response(isClose);
    httpCallback_(request, &response);

    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);

    if(response.isCloseConnection())
    {
        conn->shutdown();
    }
}