//
// Created by zhangkuo on 17-9-27.
//

#ifndef WEBSERVER_LOGGING_H
#define WEBSERVER_LOGGING_H
#include <string.h>
#include "LogStream.h"
#include "Timestamp.h"

namespace xnet
{
#define LOG_TRACE if(xnet::Logger::logLevel() <= xnet::Logger::LogLevel::TRACE) \
        xnet::Logger(__FILE__, __LINE__, xnet::Logger::LogLevel::TRACE, __FUNCTION__).stream()

#define LOG_DEBUG if(xnet::Logger::logLevel() <= xnet::Logger::LogLevel::DEBUG) \
        xnet::Logger(__FILE__, __LINE__, xnet::Logger::LogLevel::DEBUG,__FUNCTION__).stream()

#define LOG_INFO if(xnet::Logger::logLevel() <= xnet::Logger::LogLevel::INFO) \
        xnet::Logger(__FILE__, __LINE__, xnet::Logger::LogLevel::INFO).stream()

#define LOG_WARN        xnet::Logger(__FILE__, __LINE__, xnet::Logger::LogLevel::WARN).stream()
#define LOG_ERROR       xnet::Logger(__FILE__, __LINE__, xnet::Logger::LogLevel::ERROR).stream()
#define LOG_FATAL       xnet::Logger(__FILE__, __LINE__, xnet::Logger::LogLevel::FATAL).stream()
#define LOG_SYSERR      xnet::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL    xnet::Logger(__FILE__, __LINE__, true).stream()

class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };

    class SourceFile
    {
    public:
        template<int N>
        SourceFile(const char (&array)[N]):
                data_(array)
        {
            const char* slash = strrchr(data_, '/');
            if(slash)
            {
                data_ = slash + 1;
            }
            size_ = strlen(data_);
        }

        explicit SourceFile(const char* fileName):
                data_(fileName)
        {
            const char* slash = strrchr(fileName, '/');
            if(slash)
            {
                data_ = slash + 1;
            }
            size_ = strlen(data_);
        }

        const char* data_;
        size_t size_;
    };

    Logger(const SourceFile& file, int line, LogLevel level);
    Logger(const SourceFile& file, int line, LogLevel level, const char* func);
    Logger(const SourceFile& file, int line, bool toAbort);
    ~Logger();

    LogStream&          stream() { return stream_; }
    static LogLevel     logLevel();
    static void         setLogLevel(LogLevel level);

    using OutputFunc    = std::function<void(const char* msg, size_t len)>;
    using FlushFunc     = std::function<void()>;

    static void setOutput(const OutputFunc&);
    static void setFlush(const FlushFunc&);
private:
    void formatTime();
    void finish();

    LogStream   stream_;
    Timestamp   time_;
    LogLevel    level_;
    SourceFile  fileName_;
    int         line_;
};

extern Logger::LogLevel g_logLevel;
inline Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}

}

#endif //WEBSERVER_LOGGING_H
