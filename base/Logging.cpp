//
// Created by zhangkuo on 17-9-27.
//
#include "Logging.h"
using namespace xnet;

Logger::Logger(SourceFile file, int line) :
        stream_(),
        file_(file),
        line_(line)
{

}

Logger::Logger(SourceFile file, int line, LogLevel level):
        stream_(),
        file_(file),
        line_(line),
        level_(level)
{

}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func):
        stream_(),
        file_(file),
        line_(line),
        level_(level)
{

}

void Logger::setLogLevel(LogLevel level)
{
    g_logLevel = level;
}