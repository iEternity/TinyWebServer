cmake_minimum_required(VERSION 3.7)
project(WebServer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        net/main.cpp
        3rd/TinyXML2/tinyxml2.h 3rd/TinyXML2/tinyxml2.cpp
        net/Socket.h net/InetAddress.h net/InetAddress.cpp net/Endian.h
        net/SocketOps.h net/SocketOps.cpp net/Socket.cpp net/EventLoop.h
        net/EventLoop.cpp net/Channel.cpp net/Channel.h base/Timestamp.h
        base/Timestamp.cpp net/Poller.h net/Poller.cpp net/poller/PollPoller.h
        net/poller/PollPoller.cpp net/poller/EpollPoller.h
        net/poller/EpollPoller.cpp net/poller/DefaultPoller.cpp
        base/Mutex.h base/Thread.h base/Thread.cpp base/Atomic.h
        base/CurrentThread.h net/Timer.h net/Callbacks.h net/Timer.cpp
        net/TimerId.h net/TimerQueue.h net/TimerQueue.cpp net/TcpServer.h
        net/EventLoopThread.h net/EventLoopThread.cpp base/Condition.h
        net/TcpServer.cpp net/EventLoopThreadPool.h
        net/EventLoopThreadPool.cpp net/Acceptor.h net/Acceptor.cpp
        net/Buffer.h net/Buffer.cpp net/StringPiece.h net/TcpConnection.h net/TcpConnection.cpp base/WeakCallback.h net/Connector.h net/Connector.cpp net/TcpClient.h net/TcpClient.cpp)

include_directories(3rd/TinyXML2)
link_libraries(-lpthread)
add_executable(WebServer ${SOURCE_FILES})