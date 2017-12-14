add_compile_options(-std=c++11)

add_custom_command(OUTPUT rpc.pb.cc rpc.pb.h
        COMMAND protoc
        ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/rpc.proto -I${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS rpc.proto
        VERBATIM )

include_directories(${PROJECT_BINARY_DIR})

add_library(xnet_protorpc_wire rpc.pb.cc RpcCodec.cpp)
set_target_properties(xnet_protorpc_wire PROPERTIES COMPILE_FLAGS "-std=c++11")
target_link_libraries(xnet_protorpc_wire xnet_protobuf_codec)

add_executable(protobuf_rpc_wire_test RpcCodec_test.cpp)
target_link_libraries(protobuf_rpc_wire_test xnet_protorpc_wire)

add_library(xnet_protorpc RpcChannel.cpp RpcServer.cpp)
target_link_libraries(xnet_protorpc xnet_protorpc_wire xnet_protobuf_codec xnet_net protobuf z)

install(TARGETS xnet_protorpc_wire xnet_protorpc DESTINATION lib)

set(HEADERS
        RpcCodec.h
        RpcChannel.h
        RpcServer.h
        rpc.proto
        ${PROJECT_BINARY_DIR}/xnet/net/protorpc/rpc.pb.h)
install(FILES ${HEADERS} DESTINATION include/xnet/net/protorpc)