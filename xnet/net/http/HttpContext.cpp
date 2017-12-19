#include <boost/algorithm/string.hpp>
#include <vector>
#include <xnet/net/http/HttpContext.h>
using namespace std;
using namespace xnet;

bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
    bool ok = true;
    bool hasMore = true;
    while(hasMore)
    {
        if(state_ == HttpRequestParseState::kExpectRequestLine)
        {
            const char* crlf = buf->findCRLF();
            if(crlf)
            {
                ok = parseRequestLine(buf->peek(), crlf);
                if(ok)
                {
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2);
                    state_ = HttpRequestParseState::kExpectHeaders;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if(state_ == HttpRequestParseState::kExpectHeaders)
        {

        }
        else if(state_ == HttpRequestParseState::kExpectBody)
        {

        }
        else
        {
            hasMore = false;
        }
    }
}

bool HttpContext::parseRequestLine(const string& line)
{
    bool ok = true;
    boost::trim(line);

    std::vector<string> result;
    boost::split(result, line, boost::is_any_of(" "), boost::token_compress_on);

    if(result.size() == 3)
    {
        request_.setMethod(result[0]);
        request_.setPath(result[1]);
        request_.setVersion(result[2]);
    }
    else
    {
        ok = false;
    }
    return ok;
}