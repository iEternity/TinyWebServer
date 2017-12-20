#pragma once
#include <xnet/base/Types.h>
#include <map>

namespace xnet
{

class Buffer;
class HttpResponse
{
public:
    enum class HttpStatusCode
    {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404
    }
public:
    explicit HttpResponse(bool isClose):
        isCloseConnection_(isClose),
        statusCode_(HttpStatusCode::kUnknown)
    {

    }

    void setCloseConnection(bool on)
    {
        isCloseConnection_ = on;
    }

    bool isCloseConnection() const
    {
        return isCloseConnection_;
    }

    void setStatusCode(HttpStatusCode status)
    {
        statusCode_ = status;
    }

    void setStatusMessage(const string& statusMessage)
    {
        statusMessage_ = statusMessage;
    }

    void setContentType(const string& contentType)
    {
        addHeader("Content-Type", contentType);
    }

    void addHeader(const string& key, const string& val)
    {
        headers_[key] = val;
    }

    void setBody(const string& body)
    {
        body_ = body;
    }

    void appendToBuffer(Buffer* output) const;

private:
    bool isCloseConnection_;
    HttpStatusCode statusCode_;
    string statusMessage_;
    std::map<string, string> headers_;
    string body_;
};

}