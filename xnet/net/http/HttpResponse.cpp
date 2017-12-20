#include <boost/format.hpp>
#include <xnet/net/http/HttpResponse.h>
#include <xnet/net/Buffer.h>

using namespace std;
using namespace xnet;

void HttpResponse::appendToBuffer(Buffer* output) const
{
    //response line
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", static_cast<int>(statusCode_));
    output->append(buf);
    output->append(statusMessage_);
    output->append("\r\n");

    //response header
    if(isCloseConnection_)
    {
        output->append("Connection: close\r\n");
    }
    else
    {
        boost::format fmt("Content-Length: %1%\r\n");
        fmt %body_.size();
        output->append(fmt.str());
        output->append("Connection: Keep-Alive\r\n");
    }

    for(auto& it : headers_)
    {
        output->append(it.first);
        output->append(":");
        output->append(it.second);
        output->append("\r\n");
    }

    //response body
    output->append("\r\n");
    output->append(body_);
}