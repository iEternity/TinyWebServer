//
// Created by zhangkuo on 17-10-24.
//

#ifndef XNET_LOGFILE_H
#define XNET_LOGFILE_H
#include <xnet/base/noncopyable.h>
#include <xnet/base/Types.h>
#include <mutex>
#include <memory>
#include <xnet/base/FileUtil.h>
#include <time.h>

namespace xnet
{

class LogFile : noncopyable
{
public:
    LogFile(const string& basename,
            size_t rollSize,
            bool threadSafe = true,
            int flushInterval = 3,
            int checkEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    void appendUnlocked(const char* logline, int len);

    static string getLogFileName(const string& basename, time_t* now);

private:
    const string                basename_;
    const size_t                rollSize_;
    const int                   flushInterval_;
    const int                   checkEveryN_;
    int                         count_;
    std::unique_ptr<std::mutex> mutex_;
    std::unique_ptr<AppendFile> file_;
    time_t                      startOfPeriod_;
    time_t                      lastRoll_;
    time_t                      lastFlush_;

    const static int kRollPerSeconds_ = 60*60*24;
};

}

#endif //XNET_LOGFILE_H
