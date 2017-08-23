cmake_minimum_required(VERSION 3.7)
project(WebServer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        3rd/TinyXML2/tinyxml2.h 3rd/TinyXML2/tinyxml2.cpp
        Socket.h InetAddress.h InetAddress.cpp Endian.h
        SocketOps.h SocketOps.cpp Socket.cpp EventLoop.h
        EventLoop.cpp Channel.cpp Channel.h base/Timestamp.h
        base/Timestamp.cpp Poller.h Poller.cpp poller/PollPoller.h
        poller/PollPoller.cpp poller/EpollPoller.h
        poller/EpollPoller.cpp poller/DefaultPoller.cpp base/Mutex.h base/Thread.h base/Thread.cpp base/Atomic.h base/CurrentThread.h)

include_directories(3rd/TinyXML2)
link_libraries(-lpthread)
add_executable(WebServer ${SOURCE_FILES})