//
// Created by zhangkuo on 17-9-27.
//
#include <ctime>
#include "Logging.h"
#include "CurrentThread.h"

namespace xnet
{
    __thread time_t t_lastTime;
    __thread char   t_time[32];
    __thread char   t_errnoBuf[512];

    const char* strError(int savedErrno)
    {
        return strerror_r(savedErrno, t_errnoBuf, sizeof t_errnoBuf);
    }

    Logger::LogLevel initLogLevel()
    {
        if(::getenv("XNET_LOG_TRACE"))
            return Logger::LogLevel::TRACE;
        else if(::getenv("XNET_LOG_DEBUG"))
            return Logger::LogLevel::DEBUG;
        else
            return Logger::LogLevel::INFO;
    }

    const char* LogLevelName[] = {
            "TRACE ",
            "DEBUG ",
            "INFO  ",
            "WARN  ",
            "ERROR ",
            "FATAL "
    };

    inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& file)
    {
        s.append(file.data_, file.size_);
        return s;
    }

    void defaultOutput(const char* msg, size_t len)
    {
        fwrite(msg, 1, len, stdout);
    }

    void defaultFlush()
    {
        fflush(stdout);
    }

    Logger::OutputFunc g_output = defaultOutput ;
    Logger::FlushFunc  g_flush = defaultFlush;
    Logger::LogLevel   g_logLevel = initLogLevel();
}

using namespace xnet;

Logger::Logger(const SourceFile& file, int line, LogLevel level):
        stream_(),
        time_(Timestamp::now()),
        fileName_(file),
        line_(line),
        level_(level)
{
    formatTime();

    CurrentThread::tid();
    stream_ << StringPiece(CurrentThread::tidString(), CurrentThread::tidStringLength());

    stream_ << StringPiece(LogLevelName[level]);


}

Logger::Logger(const SourceFile& file, int line, LogLevel level, const char* func):
        stream_(),
        time_(Timestamp::now()),
        fileName_(file),
        line_(line),
        level_(level)
{
    formatTime();

    CurrentThread::tid();
    stream_ << StringPiece(CurrentThread::tidString(), CurrentThread::tidStringLength());

    stream_ << StringPiece(LogLevelName[level]);

    stream_ << func << ' ';
}

Logger::Logger(const SourceFile& file, int line, bool isAbort):
        stream_(),
        time_(Timestamp::now()),
        fileName_(file),
        line_(line)
{
    int savedErrno = errno;
    level_ = isAbort ? LogLevel::FATAL : LogLevel::ERROR;

    formatTime();

    CurrentThread::tid();
    stream_ << StringPiece(CurrentThread::tidString(), CurrentThread::tidStringLength());

    stream_ << StringPiece(LogLevelName[level_]);

    stream_ << strError(savedErrno) << " (errno=" << savedErrno << ") ";
}

Logger::~Logger()
{
    finish();

    const LogStream::Buffer& buf(stream_.buffer());
    g_output(buf.data(), static_cast<size_t>(buf.length()));

    if(level_ == LogLevel::FATAL)
    {
        g_flush();
        abort();
    }
}

void Logger::formatTime()
{
    int64_t microsecondsSinceEpoch = time_.microsecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microsecondsSinceEpoch / Timestamp::kMicrosecondsPerSecond);
    int microseconds = static_cast<int>(microsecondsSinceEpoch % Timestamp::kMicrosecondsPerSecond);

    if(t_lastTime != seconds)
    {
        struct tm* tmTime = std::localtime(&seconds);
        snprintf(t_time, sizeof t_time, "%4d%02d%02d %02d:%02d:%02d",
                 tmTime->tm_year + 1900, tmTime->tm_mon + 1, tmTime->tm_mday,
                 tmTime->tm_hour, tmTime->tm_min, tmTime->tm_sec
                 );
    }

    char buf[16];
    snprintf(buf, sizeof buf, ".%06d ", microseconds);
    stream_ << StringPiece(t_time, 17) << StringPiece(buf, 8);
}

void Logger::finish()
{
    stream_ << " - " << fileName_ << ':' << line_ << "\n";
}

void Logger::setLogLevel(LogLevel level)
{
    g_logLevel = level;
}

void Logger::setOutput(const OutputFunc& func)
{
    g_output = func;
}

void Logger::setFlush(const FlushFunc & func)
{
    g_flush = func;
}