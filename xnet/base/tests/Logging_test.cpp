//
// Created by zhangkuo on 17-10-13.
//
#include <xnet/base/Logging.h>
#include <unistd.h>
#include <xnet/base/ThreadPool.h>

using namespace xnet;

void logInThread()
{
    LOG_INFO << "logInThread";
    usleep(1000);
}

int main()
{
    getpid();

    ThreadPool pool("pool");
    pool.start(5);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);

    LOG_TRACE   << "trace";
    LOG_DEBUG   << "debug";
    LOG_INFO    << "This is info";
    LOG_WARN    << "This is warn";
    LOG_ERROR   << "This is error";

    LOG_INFO    << "sizeof LogLevel: "      << sizeof(Logger::LogLevel);
    LOG_INFO    << "sizeof SourceFile: "    << sizeof(Logger::SourceFile);
    LOG_INFO    << "sizeof Logger: "        << sizeof(Logger);
    LOG_INFO    << "sizeof LogStream: "     << sizeof(LogStream);
    LOG_INFO    << "sizeof Buffer: "        << sizeof(LogStream::Buffer);
}