//
// Created by zhangkuo on 17-8-7.
//
#include <cinttypes>
#include <sys/time.h>
#include "Timestamp.h"

using namespace xnet;

std::string Timestamp::toString() const
{
    char buf[32] = {0};
    int64_t seconds = microsecondsSinceEpoch_ / kMicrosecondsPerSecond;
    int64_t microseconds = microsecondsSinceEpoch_ % kMicrosecondsPerSecond;
    snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

std::string Timestamp::toFormattedString(bool showMicroseconds) const
{
    char buf[32] = {0};
    time_t seconds = static_cast<time_t>(microsecondsSinceEpoch_ / kMicrosecondsPerSecond);
    tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if(showMicroseconds)
    {
        int microseconds = static_cast<int>(microsecondsSinceEpoch_ % kMicrosecondsPerSecond);
        snprintf(buf, sizeof(buf)-1, "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 microseconds);
    }
    else{
        snprintf(buf, sizeof(buf)-1, "%4d%02d%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }

    return buf;
}

Timestamp Timestamp::now()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;

    return Timestamp(seconds * kMicrosecondsPerSecond + tv.tv_usec);
}
