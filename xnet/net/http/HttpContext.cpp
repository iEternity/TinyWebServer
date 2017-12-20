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
            const char* crlf = buf->findCRLF();
            if(crlf)
            {
                if(!request_.addHeader(buf->peek(), crlf))
                {
                    state_ = HttpRequestParseState::kGotAll;
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);
            }
            else
            {
                hasMore = false;
            }
        }
        else if(state_ == HttpRequestParseState::kExpectBody)
        {
            
        }
        else
        {
            hasMore = false;
        }
    }
    return ok;
}

bool HttpContext::parseRequestLine(const char* begin, const char* end)
{
    //例如： GET https://www.baidu.com?id=1009 HTTP/1.1
    string line(begin, end);
    bool ok = true;
    boost::trim(line);

    std::vector<string> result;
    boost::split(result, line, boost::is_any_of(" "), boost::token_compress_on);

    if(result.size() == 3)
    {
        request_.setMethod(result[0]);

        auto pos = result[1].find('?');
        if(pos == string::npos)
        {
            request_.setPath(std::move(result[1]));
        }
        else
        {
            request_.setPath(result[1].substr(0, pos));
            request_.setQuery(result[1].substr(pos+1));
        }

        assert(result[2].find('/') != string::npos);
        string v = result[2].substr(result[2].find('/'));
        if(v == "1.1")
        {
            request_.setVersion(Version::kHttp11);
        }
        else if(v == "1.0")
        {
            request_.setVersion(Version::kHttp10);
        }
    }
    else
    {
        ok = false;
    }
    return ok;
}