cmake_minimum_required(VERSION 3.7)
project(WebServer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        3rd/TinyXML2/tinyxml2.h 3rd/TinyXML2/tinyxml2.cpp Socket.h InetAddress.h InetAddress.cpp Endian.h SocketOps.h SocketOps.cpp)

include_directories(3rd/TinyXML2)

add_executable(WebServer ${SOURCE_FILES})