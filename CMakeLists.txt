cmake_minimum_required(VERSION 2.6)
project(xNet)

set(CMAKE_CXX_STANDARD 11
)

enable_testing()

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/lib)

find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)

if(Boost_FOUND)
    message(STATUS "found boost")
endif()
if(Protobuf_FOUND)
    message(STATUS "found protobuf")
endif()

include_directories(${Boost_INCLUDE_DIRECTORIES})
include_directories(${Protobuf_INCLUDE_DIRECTORIES})
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(xnet/base)
add_subdirectory(xnet/net)