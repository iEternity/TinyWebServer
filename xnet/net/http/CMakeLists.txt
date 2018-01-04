#generate lib file
set(Http_Srcs
        HttpContext.cpp
        HttpResponse.cpp
        HttpServer.cpp
        )

add_library(xnet_http ${Http_Srcs})
target_link_libraries(xnet_http xnet_net)

# install
install(TARGETS xnet_http DESTINATION lib)
set(Headers
        HttpContext.h
        HttpRequest.h
        HttpResponse.h
        )
install(FILES ${Headers} DESTINATION include/xnet/net/http)

add_executable(xnet_http_test tests/HttpServer_test.cpp)
target_link_libraries(xnet_http_test xnet_http)
