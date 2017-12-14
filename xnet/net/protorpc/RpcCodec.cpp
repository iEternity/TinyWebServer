//
// Created by zhangkuo on 17-11-30.
//
#include <xnet/net/protorpc/RpcCodec.h>
#include <xnet/net/protorpc/rpc.pb.h>

using namespace xnet;

int protobufVersionCheck()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    return 0;
}

int dummy = protobufVersionCheck();

namespace xnet
{
    const char rpcTag[] = "RPC0";
}

