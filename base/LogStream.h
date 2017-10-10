//
// Created by zhangkuo on 17-9-27.
//

#ifndef WEBSERVER_LOGSTREAM_H
#define WEBSERVER_LOGSTREAM_H
#include <stdio.h>
#include <string>
#include <functional>
#include "../net/StringPiece.h"

namespace xnet
{
namespace detail
{
const int kSmallBuffer = 4*1024;
const int kLargeBuffer = 4*1024*1024;

template<int SIZE>
class FixedBuffer
{
    using Cookie = std::function<void()>;
public:
    FixedBuffer(const FixedBuffer&) = delete;
    FixedBuffer& operator= (const FixedBuffer&) = delete;
    FixedBuffer():
            cur_(data_)
    {

    }
    ~FixedBuffer()
    {

    }

    void append(const char* buf, size_t len)
    {
        if(avail() > len)
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }
    void            add(size_t len) { cur_ += len; }
    void            reset() { cur_ = data_; }
    void            bzero() { ::bzero(data_, sizeof(data_)); }
    void            setCookie(Cookie& cookie) { cookie_ = cookie; }
    const char*     debugString();

    const char* data() const { return data_; }
    int         length() const { return static_cast<int>(cur_ - data_); }
    char*       current() { return cur_; }
    int         avail() const { return static_cast<int>(end() - cur_); }
    std::string toString() const { return std::string(data_, static_cast<size_t>(length())); }
    StringPiece toStringPiece() const { return StringPiece(data_, static_cast<size_t>(length())); }

private:
    const char* end() const { return data_ + sizeof(data_);}
    static void cookieStart();
    static void cookieEnd();

    Cookie  cookie_;
    char    data_[SIZE];
    char*   cur_;
};
}

class LogStream
{
public:
    using Buffer = detail::FixedBuffer<detail::kSmallBuffer>;

    LogStream() = default;
    ~LogStream() = default;
    LogStream(const LogStream& ) = delete;
    LogStream& operator= (const LogStream&) = delete;

    LogStream& operator<< (bool v)
    {
        buffer_.append((v ? "1" : "0"), 1);
        return *this;
    }

    LogStream& operator<< (short);
    LogStream& operator<< (unsigned short);
    LogStream& operator<< (int);
    LogStream& operator<< (unsigned int);
    LogStream& operator<< (long);
    LogStream& operator<< (unsigned long);
    LogStream& operator<< (long long);
    LogStream& operator<< (unsigned long long);

    LogStream& operator<< (const void*);

    LogStream& operator<< (float v)
    {
        *this << static_cast<double>(v);
    }
    LogStream& operator<< (double v);

    LogStream& operator<< (char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }
    LogStream& operator<< (const char* str)
    {
        if(str)
        {
            buffer_.append(str, strlen(str));
        }
        else
        {
            buffer_.append("(null)", 6);
        }
        return *this;
    }
    LogStream& operator<< (const unsigned char* str)
    {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    LogStream& operator<< (const std::string& str)
    {
        buffer_.append(str.data(), str.size());
        return *this;
    }
    LogStream& operator<< (const StringPiece& str)
    {
        buffer_.append(str.data(), str.size());
        return *this;
    }
    LogStream& operator<< (const Buffer& buf)
    {
        return *this << buf.toStringPiece();
    }

    void append(const char* data, size_t len)
    {
        buffer_.append(data, len);
    }
    const Buffer& buffer() const
    {
        return buffer_;
    }
    void resetBuffer()
    {
        buffer_.reset();
    }
private:
    template<typename T>
    void formatInteger(T);

    Buffer buffer_;
    static constexpr int kMaxNumericSize = 32;
};

}

#endif //WEBSERVER_LOGSTREAM_H
