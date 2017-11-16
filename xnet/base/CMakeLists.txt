add_compile_options(-std=c++11)

set(base_srcs
        AsyncLogging.cpp
        CountDownLatch.cpp
        Exception.cpp
        FileUtil.cpp
        Logging.cpp
        LogFile.cpp
        LogStream.cpp
        ProcessInfo.cpp
        Thread.cpp
        ThreadPool.cpp
        Timestamp.cpp
)

add_library(xnet_base ${base_srcs})
target_link_libraries(xnet_base pthread)

install(TARGETS xnet_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/xnet/base)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()