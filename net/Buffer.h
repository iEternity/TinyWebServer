//
// Created by zhangkuo on 17-9-16.
//

#ifndef WEBSERVER_BUFFER_H
#define WEBSERVER_BUFFER_H

#include <vector>
#include <stdio.h>
#include <algorithm>
#include <assert.h>
#include <cstring>
#include "StringPiece.h"
#include "Endian.h"

namespace WebServer
{

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize):
            buffer_(kCheapPrepend + kInitialSize),
            readerIndex_(kCheapPrepend),
            writerIndex_(kCheapPrepend)
    {

    }

    void swap(Buffer& rhs)
    {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_, rhs.readerIndex_);
        std::swap(writerIndex_, rhs.writerIndex_);
    }

    size_t readableBytes() const
    {
        return writerIndex_ - readerIndex_;
    }

    size_t writableBytes() const
    {
        return buffer_.size() - writerIndex_;
    }

    size_t prependableBytes() const
    {
        return readerIndex_;
    }

    const char* peek() const
    {
        return begin() + readerIndex_;
    }

    const char* findCRLF() const
    {
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findCRLF(const char* start) const
    {
        assert(start >= peek());
        assert(start <= beginWrite());
        const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findEOL() const
    {
        const void* eof = memchr(peek(), '\n', readableBytes());
        return static_cast<const char*>(eof);
    }

    const char* findEOL(const char* start)
    {
        assert(start >= peek());
        assert(start <= beginWrite());
        const void* eof = memchr(start, '\n', readableBytes());
        return static_cast<const char*>(eof);
    }

    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if(len <readableBytes())
        {
            readerIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }

    void retrieveAll()
    {
        readerIndex_ = writerIndex_ = kCheapPrepend;
    }

    void retrieveUntil(const char* end)
    {
        assert(end <= beginWrite());
        assert(end >= peek());
        retrieve(end - peek());
    }

    void retrieveInt64()
    {
        retrieve(sizeof (int64_t));
    }

    void retrieveInt32()
    {
        retrieve(sizeof (int32_t));
    }

    void retrieveInt16()
    {
        retrieve(sizeof (int16_t));
    }

    void retrieveInt8()
    {
        retrieve(sizeof (int8_t));
    }

    std::string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);

        return result;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    StringPiece toStringPiece() const
    {
        StringPiece(peek(), readableBytes());
    }

    void append(const StringPiece& str)
    {
        append(str.data(), str.size());
    }

    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWriten(len);
    }

    void append(const void* data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    void ensureWritableBytes(size_t len)
    {
        if(writableBytes() < len)
        {
            makeSpace(len);
        }
    }

    char* beginWrite()
    {
        return begin() + writerIndex_;
    }

    const char* beginWrite() const
    {
        return begin() + writerIndex_;
    }

    void hasWriten(size_t len)
    {
        writerIndex_ += len;
    }

    void unwrite(size_t len)
    {
        writerIndex_ -= len;
    }

    void appendInt64(int64_t x)
    {
        int64_t be64 = sockets::hostToNetwork64(x);
        append(&be64, sizeof be64);
    }

    void appendInt32(int32_t x)
    {
        int32_t be32 = sockets::hostToNetwork32(x);
        append(&be32, sizeof be32);
    }

    void appendInt16(int16_t x)
    {
        int16_t be16 = sockets::hostToNetwork16(x);
        append(&be16, sizeof be16);
    }

    void appendInt8(int8_t x)
    {
        append(&x, sizeof x);
    }

    int64_t readInt64()
    {
        int64_t result = peekInt64();
        retrieveInt64();
        return result;
    }

    int32_t readInt32()
    {
        int32_t result = peekInt32();
        retrieveInt32();
        return result;
    }

    int16_t readInt16()
    {
        int16_t result = peekInt16();
        retrieveInt16();
        return result;
    }

    int8_t readInt8()
    {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }

    int64_t peekInt64() const
    {
        int64_t be64 = 0;
        memcpy(&be64, peek(), sizeof be64);
        return sockets::networkToHost64(be64);
    }

    int32_t peekInt32() const
    {
        int32_t be32 = 0;
        memcpy(&be32, peek(), sizeof be32);
        return sockets::networkToHost32(be32);
    }

    int16_t peekInt16() const
    {
        int16_t be16 = 0;
        memcpy(&be16, peek(), sizeof be16);
        return sockets::networkToHost16(be16);
    }

    int8_t peekInt8() const
    {
        int8_t be8 = 0;
        memcpy(&be8, peek(), sizeof be8);
        return be8;
    }

    void prependInt64(int64_t x)
    {
        int64_t be64 = sockets::hostToNetwork64(x);
        prepend(&be64, sizeof be64);
    }

    void prependInt32(int32_t x)
    {
        int32_t be32 = sockets::hostToNetwork32(x);
        prepend(&be32, sizeof be32);
    }

    void prepend16(int16_t x)
    {
        int16_t be16 = sockets::hostToNetwork16(x);
        prepend(&be16, sizeof be16);
    }

    void prepend8(int8_t x)
    {
        prepend(&x, sizeof x);
    }

    void prepend(const void* data, size_t len)
    {
        readerIndex_ -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d + len, begin() + readerIndex_);
    }

    void shrink(size_t reserve)
    {
        Buffer other;
        ensureWritableBytes(readableBytes() + reserve);
        other.append(toStringPiece());
        swap(other);
    }

    size_t internalCapacity() const
    {
        return buffer_.capacity();
    }

    ssize_t readFd(int fd, int* savedErrno);

private:
    char* begin()
    {
        return buffer_.data();
    }

    const char* begin() const
    {
        return buffer_.data();
    }

    void makeSpace(size_t len)
    {
        if(prependableBytes() + writableBytes() < kCheapPrepend + len)
        {
            buffer_.resize(writerIndex_ + len);
        }
        else
        {
            size_t readable= readableBytes();
            std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }
private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
    static const char kCRLF[];
};

}

#endif //WEBSERVER_BUFFER_H
