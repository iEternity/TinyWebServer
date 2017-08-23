//
// Created by zhangkuo on 17-8-23.
//

#ifndef WEBSERVER_CALLBACKS_H
#define WEBSERVER_CALLBACKS_H

#include <functional>

namespace WebServer
{

class Buffer;
class TcpConnection;

using TimerCallback = std::function<void()>;

}

#endif //WEBSERVER_CALLBACKS_H
