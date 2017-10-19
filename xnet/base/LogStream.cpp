//
// Created by zhangkuo on 17-9-27.
//
#include "LogStream.h"
#include <algorithm>
using namespace xnet;
using namespace xnet::detail;

namespace xnet
{
namespace detail
{
const char digits[] = "9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9";
const char* zero = &digits[9];
const char digitsHex[] = "0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F";

template<typename T>
size_t convert(char buf[], T value)
{
    int rest = 0;
    char* p = buf;
    T i = value;
    while(i != 0)
    {
        rest = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[rest];
    }

    if(value < 0) *p++ = '-';
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

template<typename T>
size_t convertHex(char buf[], T v)
{
    T i = v;
    char* p = buf;
    int rest = 0;
    while(i != 0)
    {
        rest = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[rest];
    }

    *p = '\0';
    std::reverse(buf, p);
    return p-buf;
}

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

}
}

template<int SIZE>
const char* FixedBuffer<SIZE>::debugString()
{
    *cur_ = '\0';
    return data_;
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieStart()
{

}

template<int SIZE>
void  FixedBuffer<SIZE>::cookieEnd()
{

}

template<typename T>
void LogStream::formatInteger(T v)
{
    if(buffer_.avail() >= kMaxNumericSize)
    {
        size_t len = detail::convert(buffer_.current(), v);
        buffer_.add(len);
    }
}

LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if(buffer_.avail() >= kMaxNumericSize)
    {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(static_cast<size_t>(len));
    }

    return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
    if( buffer_.avail() >= kMaxNumericSize)
    {
        char* buf = buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        uintptr_t v = reinterpret_cast<uintptr_t>(p);
        size_t len = convertHex(buf + 2, v);
        buffer_.add(len + 2);
    }

    return *this;
}