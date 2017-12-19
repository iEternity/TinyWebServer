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

    bool isGotAll() const { state_ == HttpRequestParseState::kGotAll; }
    const HttpRequest& request() const { return request_; }
    HttpRequest& request() const { return request_; }

private:
    bool parseRequestLine(const string& line);

private:
    HttpRequestParseState state_;
    HttpRequest request_;
};

}