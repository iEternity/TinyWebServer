//
// Created by zhangkuo on 17-10-24.
//
#include <xnet/base/LogFile.h>
#include <xnet/base/ProcessInfo.h>
using namespace xnet;
using namespace xnet::FileUtil;

LogFile::LogFile(const string &basename,
                 size_t rollSize,
                 bool threadSafe,
                 int flushInterval,
                 int checkEveryN) :
        basename_(basename),
        rollSize_(rollSize),
        flushInterval_(flushInterval),
        checkEveryN_(checkEveryN),
        mutex_(threadSafe ? new std::mutex() : nullptr),
        startOfPeriod_(0),
        lastRoll_(0),
        lastFlush_(0)
{
    rollFile();
}

LogFile::~LogFile()
{

}

void LogFile::append(const char* logline, int len)
{
    if(mutex_ != nullptr)
    {
        std::unique_lock<std::mutex> lock(*mutex_);
        appendUnlocked(logline, len);
    }
    else
    {
        appendUnlocked(logline, len);
    }
}

void LogFile::appendUnlocked(const char* logline, int len)
{
    file_->append(logline, len);

    if(file_->writtenBytes() > rollSize_)
    {
        rollFile();
    }
    else
    {
        ++count_;
        if(count_ >= checkEveryN_)
        {
            count_ = 0;
            time_t now = ::time(nullptr);
            time_t thisPeriod = now / kRollPerSeconds_ * kRollPerSeconds_;
            if(thisPeriod != startOfPeriod_)
            {
                rollFile();
            }
            else if(now - lastFlush_ > flushInterval_)
            {
                lastFlush_ = now;
                file_->flush();
            }
        }
    }
}

void LogFile::flush()
{
    if(mutex_ != nullptr)
    {
        std::lock_guard<std::mutex> lock(*mutex_);
        file_->flush();
    }
    else
    {
        file_->flush();
    }
}

bool LogFile::rollFile()
{
    time_t now = 0;
    string filename = getLogFileName(basename_, &now);

    if(now > lastRoll_)
    {
        time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new AppendFile(filename));
        return true;
    }

    return false;
}

string LogFile::getLogFileName(const string& basename, time_t* now)
{
    string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timeBuf[32];
    struct tm tmTime;
    *now = time(nullptr);
    ::localtime_r(now, &tmTime);
    strftime(timeBuf, sizeof(timeBuf), ".%Y%m%d-%H%M%S.", &tmTime);
    filename += timeBuf;

    filename += ProcessInfo::hostname();

    char pidBuf[32];
    snprintf(pidBuf, sizeof(pidBuf), ".%d", ProcessInfo::pid());
    filename += pidBuf;

    filename += ".log";

    return filename;
}