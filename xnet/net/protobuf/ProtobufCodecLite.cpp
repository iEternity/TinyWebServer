//
// Created by zhangkuo on 17-11-20.
//
#include <google/protobuf/message.h>
#include <zlib.h>
#include <xnet/net/protobuf/ProtobufCodecLite.h>
#include <xnet/net/TcpConnection.h>

using namespace xnet;

void ProtobufCodecLite::send(const TcpConnectionPtr& conn, const Message& message)
{
    Buffer buf;
    fillEmptyBuffer(&buffer, message);
    conn->send(&buf);
}

void ProtobufCodecLite::fillEmptyBuffer(Buffer* buffer, const Message& message)
{
    buffer->append(tag_);

    int size = serializeToBuffer(message, buffer);

    int32_t checksum = checksum(buffer->peek(), buffer->readableBytes());
    buffer->appendInt32(checksum);

    int32_t len = sockets::hostToNetwork32(buffer->readableBytes());
    buffer->prepend(&len, sizeof len);
}

int ProtobufCodecLite::serializeToBuffer(const Message& message, Buffer* buffer)
{
    int size = message.ByteSize();
    buffer->ensureWritableBytes(size + kCheckSumLen);

    uint8_t* start  = static_cast<uint8_t*>(buffer->beginWrite());
    uint8_t* end    = message.SerializeWithCachedSizesToArray(start);

    if(end - start != size)
    {
        ByteSizeConsistencyError(size, message.ByteSize(), static_cast<size_t>(end - start));
    }

    buffer->hasWriten(size);

    return size;
}

int32_t ProtobufCodecLite::checksum(const void* data, size_t len)
{
    return static_cast<int32_t>(::adler32(1, static_cast<Bytef*>(data), len));
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp)
{

}