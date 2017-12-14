//
// Created by zhangkuo on 17-11-30.
//

#ifndef XNET_RPCCODEC_H
#define XNET_RPCCODEC_H
#include <memory>
#include <xnet/net/protobuf/ProtobufCodecLite.h>

namespace xnet
{

class Buffer;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

class RpcMessage;
using RpcMessagePtr = std::shared_ptr<RpcMessage>;
extern const char rpcTag[];// = "RPC0"

/*
 * Data Format
 *
 * Field    Length      Content
 *
 * size     4-byte      N+8
 * "RPC0"   4-byte
 * payload  N-byte
 * checksum 4-byte
 */

using RpcCodec = ProtobufCodecLiteT<RpcMessage, rpcTag>;
}

#endif //XNET_RPCCODEC_H
