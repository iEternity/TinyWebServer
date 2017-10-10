//
// Created by zhangkuo on 17-9-27.
//

#ifndef WEBSERVER_LOGGING_H
#define WEBSERVER_LOGGING_H
#include <string.h>
#include "LogStream.h"

namespace xnet
{
#define LOG_TRACE if(Logger::LogLevel() <= Logger::LogLevel::TRACE) \
        Logger(__FILE__, __LINE__, Logger::LogLevel::TRACE, __function__).stream()
#define LOG_DEBUG if(Logger::LogLevel() <= Logger::LogLevel::DEBUG) \
        Logger(__FILE__, __LINE__, Logger::LogLevel::DEBUG,__function__).stream()
#define LOG_INFO if(Logger::LogLevel() <= Logger::LogLevel::INFO) \
        Logger(__FILE__, __LINE__).stream()


class Logger
{
public:
    enum class LogLevel
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
            size_ = static_cast<int>(strlen(data_));
        }

        explicit SourceFile(const char* fileName):
                data_(fileName)
        {
            const char* slash = strrchr(fileName, '/');
            if(slash)
            {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream&          stream() { return stream_; }
    static LogLevel     logLevel();
    static void         setLogLevel(LogLevel level);

private:
    LogStream   stream_;
    LogLevel    level_;
    SourceFile  file_;
    int         line_;
};

extern Logger::LogLevel g_logLevel;
Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}

}

#endif //WEBSERVER_LOGGING_H
