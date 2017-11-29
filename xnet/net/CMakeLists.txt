add_compile_options(-std=c++11)

set(net_srcs
        poller/DefaultPoller.cpp
        poller/EpollPoller.cpp
        poller/PollPoller.cpp
        Acceptor.cpp
        Buffer.cpp
        Channel.cpp
        Connector.cpp
        EventLoop.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
        InetAddress.cpp
        Poller.cpp
        Socket.cpp
        SocketOps.cpp
        TcpClient.cpp
        TcpConnection.cpp
        TcpServer.cpp
        Timer.cpp
        TimerQueue.cpp
)

add_library(xnet_net ${net_srcs})
target_link_libraries(xnet_net xnet_base rt)

install(TARGETS xnet_net DESTINATION lib)

set(headers
        poller/EpollPoller.h
        poller/PollPoller.h
        Acceptor.h
        Buffer.h
        Callbacks.h
        Channel.h
        Connector.h
        Endian.h
        EventLoop.h
        EventLoopThread.h
        EventLoopThreadPool.h
        InetAddress.h
        Poller.h
        Socket.h
        SocketOps.h
        ../base/StringPiece.h
        TcpClient.h
        TcpConnection.h
        TcpServer.h
        Timer.h
        TimerId.h
        TimerQueue.h
)
install(FILES ${headers} DESTINATION include/xnet/net)

add_subdirectory(protobuf)