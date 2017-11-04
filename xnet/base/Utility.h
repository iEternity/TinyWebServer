//
// Created by zhangkuo on 17-10-22.
//
#ifndef XNET_UTILITY_H
#define XNET_UTILITY_H
#include <string.h>
#include <time.h>

namespace xnet
{
namespace utility
{
    static __thread char t_errorBuf[512];
    inline const char* errnoToString(int savedErrno)
    {
        return strerror_r(savedErrno, t_errorBuf, sizeof(t_errorBuf));
    }

    static __thread char t_time[32];
    inline const char* formatTime(int64_t seconds)
    {
        struct tm tmTime;
        ::localtime_r(&seconds, &tmTime);

        strftime(t_time, sizeof(t_time), "%Y%m%d %T", &tmTime);
//        snprintf(t_time, sizeof t_time, "%4d%02d%02d %02d:%02d:%02d",
//                 tmTime->tm_year + 1900, tmTime->tm_mon + 1, tmTime->tm_mday,
//                 tmTime->tm_hour, tmTime->tm_min, tmTime->tm_sec
//        );
        return t_time;
    }
}

}

#endif //XNET_UTILITY_H
