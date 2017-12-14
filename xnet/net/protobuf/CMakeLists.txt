add_library(xnet_protobuf_codec ProtobufCodecLite.cpp)
set_target_properties(xnet_protobuf_codec PROPERTIES COMPILE_FLAGS "-std=c++11")
target_link_libraries(xnet_protobuf_codec xnet_net protobuf z)

install(TARGETS xnet_protobuf_codec DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/xnet/net/protobuf)