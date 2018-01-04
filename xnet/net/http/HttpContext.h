#pragma once
#include <xnet/base/Timestamp.h>
#include <xnet/net/Buffer.h>
#include <xnet/net/http/HttpRequest.h>

namespace xnet
{

class HttpContext
{
public:
    enum class HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll
    };
public:
    HttpContext():
        state_(HttpRequestParseState::kExpectRequestLine)
    {
    }

    bool parseRequest(Buffer* buf, Timestamp receiveTime);

    bool hasGotAll() const { state_ == HttpRequestParseState::kGotAll; }
    const HttpRequest& request() const { return request_; }
    HttpRequest& request() { return request_; }

    void reset()
    {
        state_ = HttpRequestParseState::kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

private:
    bool parseRequestLine(const char* begin, const char* end);

private:
    HttpRequestParseState state_;
    HttpRequest request_;
};

}