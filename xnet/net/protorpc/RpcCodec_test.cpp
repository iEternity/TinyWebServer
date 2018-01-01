//
// Created by zhangkuo on 17-12-10.
//
#include <string>
#include <assert.h>
#include <xnet/net/protorpc/RpcCodec.h>
#include <xnet/net/Buffer.h>
#include <xnet/net/protorpc/rpc.pb.h>
#include <xnet/net/protobuf/ProtobufCodecLite.h>

using namespace std;
using namespace xnet;

MessagePtr g_messagePtr;

void onRpcMessage(const TcpConnectionPtr& conn, const RpcMessagePtr& message, Timestamp receiveTime)
{

}

void onMessage(const TcpConnectionPtr& conn, const MessagePtr& message, Timestamp receiveTime)
{
    g_messagePtr = message;
}

void print(const Buffer& buf)
{
    printf("encoded to %zd bytes\n", buf.readableBytes());
    for(size_t i=0; i<buf.readableBytes(); i++)
    {
        unsigned char ch = static_cast<unsigned char>(buf.peek()[i]);

        printf("%2zd: 0x%02x  %c\n", i, ch, isgraph(ch) ? ch : ' ');
    }
}

int main()
{
    RpcMessage message;
    message.set_type(REQUEST);
    message.set_id(2);

    char data[] = "\0\0\0\x13" "RPC0" "\x08\x01\x11\x02\0\0\0\0\0\0\0" "\x0f\xef\x01\x32";
    string excepted(data, sizeof(data) - 1);
    string s1, s2;
    Buffer buf1, buf2;
    {
        RpcCodec codec(onRpcMessage);
        codec.fillEmptyBuffer(&buf1, message);
        print(buf1);
        s1 = buf1.toStringPiece().asString();

        assert(excepted == s1);
    }

    {
        ProtobufCodecLite codec(&RpcMessage::default_instance(), "RPC0", onMessage);
        codec.fillEmptyBuffer(&buf2, message);
        print(buf2);
        s2 = buf2.toStringPiece().asString();

        assert(excepted == s2);

        codec.onMessage(TcpConnectionPtr(), &buf1, Timestamp::now());
        assert(g_messagePtr);
        assert(g_messagePtr->DebugString() == message.DebugString());

        g_messagePtr.reset();
    }

    ::google::protobuf::ShutdownProtobufLibrary();
}