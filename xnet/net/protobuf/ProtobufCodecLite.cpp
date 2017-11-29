//
// Created by zhangkuo on 17-11-20.
//
#include <map>
#include <google/protobuf/message.h>
#include <zlib.h>
#include <xnet/net/protobuf/ProtobufCodecLite.h>
#include <xnet/net/TcpConnection.h>
#include <xnet/base/Logging.h>

using namespace xnet;

void ProtobufCodecLite::send(const TcpConnectionPtr& conn, const Message& message)
{
    Buffer buf;
    fillEmptyBuffer(&buf, message);
    conn->send(&buf);
}

void ProtobufCodecLite::fillEmptyBuffer(Buffer* buffer, const Message& message)
{
    buffer->append(tag_);

    int size = serializeToBuffer(message, buffer);

    int32_t checksumT = checksum(buffer->peek(), buffer->readableBytes());
    buffer->appendInt32(checksumT);

    int32_t len = sockets::hostToNetwork32(buffer->readableBytes());
    buffer->prepend(&len, sizeof len);
}

bool ProtobufCodecLite::parseFromBuffer(const StringPiece& buffer, Message* message)
{
    return message->ParseFromArray(buffer.data(), buffer.size());
}

int ProtobufCodecLite::serializeToBuffer(const Message& message, Buffer* buffer)
{
    int size = message.ByteSize();
    buffer->ensureWritableBytes(size + kChecksumLen);

    uint8_t* start  = reinterpret_cast<uint8_t*>(buffer->beginWrite());
    uint8_t* end    = message.SerializeWithCachedSizesToArray(start);

    if(end - start != size)
    {
        //ByteSizeConsistencyError(size, message.ByteSize(), static_cast<size_t>(end - start));
        LOG_ERROR << "ProtobufCodecLite::serializeToBuffer ByteSizeConsistencyError";
    }

    buffer->hasWriten(size);

    return size;
}

void ProtobufCodecLite::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime)
{
    while(buffer->readableBytes() >= static_cast<uint32_t>(kHeaderLen + kMinMessageLen))
    {
        const int32_t len = buffer->peekInt32();
        if(len < kMinMessageLen || len > kMaxMessageLen)
        {
            errorCallback_(conn, buffer, receiveTime, ErrorCode::kInvalidLength);
            break;
        }
        else if(len >= static_cast<uint32_t>(buffer->readableBytes()))
        {
            if(rawMessageCallback_ && rawMessageCallback_(conn, StringPiece(buffer->peek(), len + kHeaderLen), receiveTime))
            {
                buffer->retrieve(kHeaderLen + len);
                continue;
            }
            MessagePtr message(prototype_->New());
            ErrorCode error = parse(buffer->peek() + kHeaderLen, len, message.get());
            if(error == ErrorCode::kNoError)
            {
                messageCallback_(conn, message, receiveTime);
                buffer->retrieve(len + kHeaderLen);
            }
            else
            {
                errorCallback_(conn, buffer, receiveTime, error);
                break;
            }
        }
        else
        {
            break;
        }
    }
}

ProtobufCodecLite::ErrorCode ProtobufCodecLite::parse(const char* data, int len, Message* message)
{
    ErrorCode error = ErrorCode::kNoError;
    if(validateChecksum(data, len))
    {
        if(memcmp(data, tag_.data(), tag_.size()) == 0)
        {
            const char* d = data + tag_.size();
            size_t dataLen = len - tag_.size() - kChecksumLen;
            if(parseFromBuffer(StringPiece(data, dataLen), message))
            {
                error = ErrorCode::kNoError;
            }
            else
            {
                error = ErrorCode::kParseError;
            }
        }
        else
        {
            error = ErrorCode::kUnknownMessageType;
        }
    }
    else
    {
        error = ErrorCode::kChecksumError;
    }
    return error;
}

int32_t ProtobufCodecLite::checksum(const void* data, size_t len)
{
    return static_cast<int32_t>(::adler32(1, static_cast<const Bytef*>(data), len));
}

bool ProtobufCodecLite::validateChecksum(const char* data, size_t len)
{
    int32_t exceptedChecksum = asInt32(data + len - kChecksumLen);
    int32_t checksumT = checksum(data, len);

    return checksumT == exceptedChecksum;
}

int32_t ProtobufCodecLite::asInt32(const char* data)
{
    int32_t ret;
    ::memcpy(&ret, data, sizeof(ret));
    return sockets::networkToHost32(ret);
}

std::map<ProtobufCodecLite::ErrorCode, std::string> errorMap =
{
    {ProtobufCodecLite::ErrorCode::kNoError,               "NoError"},
    {ProtobufCodecLite::ErrorCode::kChecksumError,         "ChecksumError"},
    {ProtobufCodecLite::ErrorCode::kInvalidLength,         "InvalidLength"},
    {ProtobufCodecLite::ErrorCode::kInvalidNameLen,        "InvalidNameLen"},
    {ProtobufCodecLite::ErrorCode::kParseError,            "ParseError"},
    {ProtobufCodecLite::ErrorCode::kUnknownMessageType,    "UnknownMessageType"}
};
const std::string& ProtobufCodecLite::errorCodeToString(ErrorCode error)
{
    return errorMap[error];
}

void ProtobufCodecLite::defaultErrorCallback(const TcpConnectionPtr& conn, 
                                             Buffer* buffer, 
                                             Timestamp receiveTime, 
                                             ErrorCode error)
{
    LOG_ERROR << "ProtobufCodecLite::defaultErrorCallback - " << errorCodeToString(error);
    if(conn && conn->connected())
    {
        conn->shutdown();
    }
}