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

    Field      Length      Content
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

    static const int kHeaderLen = sizeof(int32_t);
    static const int kChecksumLen = sizeof(int32_t);
    static const int kMaxMessageLen = 64*1024*1024;

    using Message = ::google::protobuf::Message;
    using RawMessageCallback = std::function<bool (const TcpConnectionPtr&, 
                                                   const StringPiece&, 
                                                   Timestamp)>;

    using ProtobufMessageCallback = std::function<void (const TcpConnectionPtr&,
                                                        const MessagePtr&, 
                                                        Timestamp)>;

    using ErrorCallback = std::function<void (const TcpConnectionPtr&,
                                              Buffer*,
                                              Timestamp,
                                              ErrorCode)>;
public:
    ProtobufCodecLite(const Message* prototype, 
                      const StringPiece& tag, 
                      const ProtobufMessageCallback& messageCb,
                      const RawMessageCallback& rawMessageCb = RawMessageCallback(),
                      const ErrorCallback& errorCb = defaultErrorCallback) :
        prototype_(prototype),
        tag_(tag.asString()),
        messageCallback_(messageCb),
        rawMessageCallback_(rawMessageCb),
        errorCallback_(errorCb),
        kMinMessageLen(tag.size() + kChecksumLen)
    {
    }

    virtual ~ProtobufCodecLite() = default;

    const std::string& tag() const { return tag_; }

    void send(const TcpConnectionPtr& conn, const Message& message);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);

    virtual bool parseFromBuffer(const StringPiece& buffer, Message* message);
    virtual int serializeToBuffer(const Message& message, Buffer* buffer);

    static const std::string& errorCodeToString(ErrorCode code);

    // public for test
    void fillEmptyBuffer(Buffer* buffer, const Message& message);

    static int32_t checksum(const void* data, size_t len);
    static bool validateChecksum(const char* data, size_t len);
    static int32_t asInt32(const char* data);

    ErrorCode parse(const char* buf, int len, Message* message);
    static void defaultErrorCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime, ErrorCode error);

private:
    const Message* prototype_;
    const std::string tag_;
    ProtobufMessageCallback messageCallback_;
    RawMessageCallback rawMessageCallback_;
    ErrorCallback errorCallback_;
    const int kMinMessageLen;
};

    // TAG is not a string literal
template<typename MSG, const char* TAG, typename CODEC = ProtobufCodecLite>
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
    explicit ProtobufCodecLiteT(const ProtobufMessageCallback& messageCb,
                                const RawMessageCallback& rawCb = RawMessageCallback(),
                                ErrorCallback errorCb = ProtobufCodecLite::defaultErrorCallback):
            messageCallback_(messageCb),
            codec_(&MSG::default_instance(),
                   TAG,
                    std::bind(&ProtobufCodecLiteT::onRpcMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                    rawCb,
                    errorCb)
    {

    }

    const std::string& tag() const { return codec_.tag(); }

    void send(const TcpConnectionPtr& conn, const MSG& message)
    {
        codec_.send(conn, message);
    }

    void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf,
                   Timestamp receiveTime)
    {
        codec_.onMessage(conn, buf, receiveTime);
    }

    void onRpcMessage(const TcpConnectionPtr& conn,
                      const MessagePtr& message,
                      Timestamp receiveTime)
    {
        messageCallback_(conn, std::static_pointer_cast<MSG>(message), receiveTime);
    }

    void fillEmptyBuffer(Buffer* buf, const MSG& message)
    {
        codec_.fillEmptyBuffer(buf, message);
    }

private:
    ProtobufMessageCallback messageCallback_;
    CODEC codec_;
};

}