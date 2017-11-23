#pragma once
#include <memory>
#include <xnet/base/noncopyable.h>
#include <xnet/base/StringPiece.h>
#include <xnet/base/Timestamp.h>

namespace google
{
namespace protobuf
{
    class Message;
}
}

namespace xnet
{

class TcpConnection;
class Buffer;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using MessagePtr = std::shared_ptr<::google::protobuf::Message>;

/*
    DATA FORMAT

    Fileld      Length      Content
    size        4-byte      M+N+4
    tag         M-byte      could be "RPC0",etc.
    payload     N-byte  
    checksum    4-byte      adler32 of tag+payload
*/

//This is an internal class, you should use ProtobufCodecLiteT instead.
class ProtobufCodecLite : noncopyable
{
public:
    enum class ErrorCode
    {
        kNoError = 0,
        kInvalidLength,
        kChecksumError,
        kInvalidNameLen,
        kUnknownMessageType,
        kParseError
    };

    using Message = ::google::protobuf::Message;
    using RawMessageCallback = std::function<bool (const TcpConnectionPtr&, 
                                                   const StringPiece&, 
                                                   Timestamp)>;

    using ProtobufMessageCallback = std::function<void (const TcpConnectionPtr&,
                                                        const MessagePtr&, 
                                                        Timestamp)>;

    using ErrorCallback = std::function<void (const TcpConenctionPtr&,
                                              Buffer* ,
                                              Timestamp,
                                              ErrorCode)>;                                                        
public:
    ProtobufCodecLite(const Message* prototype, 
                      const StringPiece& tag, 
                      const ProtobufMessageCallback& messageCb,
                      const RawMessageCallback& rawMessageCb = RawMessageCallback(),
                      const ErrorCallback& errorCb = ErrorCallback()) :
        prototype_(prototype),
        tag_(tag.asString()),
        messageCallback_(messageCb),
        rawMessageCallback_(rawMessageCb),
        errorCallback_(errorCallback),
        kMinMessageLen(tag.size() + kChecksumLen)
    {
    }

    virtual ~ProtobufCodecLite() = default;

    const std::string& tag() const { return tag_; }

    void send(const TcpConnectionPtr& conn, const Message& message);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);

    virtual bool parseFromBuffer(const StringPiece& buffer, Message* message);
    virtual int serializeToBuffer(const Message& message, Buffer* buffer);

    const std::string& errorCodeToString(ErrorCode code);

    // public for test
    void fillEmptyBuffer(Buffer* buffer, const Message& message);

    static int32_t checksum(const void* data, size_t len);
    static bool validateChecksum(const char* data, int size_t len);
    static int32_t asInt32(const char* data);

    ErrorCode parse(const char* buf, int len, Message* message);
    static void defaultErrorCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime, ErrorCode error);

private:
    static const int kHeaderLen = sizeof(int32_t);
    static const int kChecksumLen = sizeof(int32_t);
    static cosnt int kMaxMessageLen = 64*1024*1024;

    const Message* prototype_;
    const std::string tag_;
    ProtobufMessageCallback messageCallback_;
    RawMessageCallback rawMessageCallback_;
    ErrorCallback errorCallback_;
    const int kMinMessageLen;
};

template<typename MSG, typename const char* TAG, typename CODEC = ProtobufCodecLite>
class ProtobufCodecLiteT
{
public:
    using ConcreteMessagePtr = std::shared_ptr<MSG>;
    using ProtobufMessageCallback = std::function<void (const TcpConnectionPtr& conn,
                                                        const ConcreteMessagePtr& message,
                                                        Timestamp)>;
    using RawMessageCallback = ProtobufCodecLite::RawMessageCallback;
    using ErrorCallback = ProtobufCodecLite::ErrorCallback;
    
public:
    explicit ProtobufCodecLiteT()

}

}