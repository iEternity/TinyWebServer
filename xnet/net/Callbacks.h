//
// Created by zhangkuo on 17-8-23.
//

#ifndef XNET_CALLBACKS_H
#define XNET_CALLBACKS_H

#include <functional>
#include <memory>

namespace xnet
{

class Buffer;
class TcpConnection;
class Timestamp;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using TimerCallback = std::function<void()>;

using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr&, size_t)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&,
                                           Buffer*,
                                           Timestamp)>;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);

}

#endif //XNET_CALLBACKS_H
