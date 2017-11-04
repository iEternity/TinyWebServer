//
// Created by zhangkuo on 17-10-26.
//

#ifndef XNET_PROCESSINFO_H
#define XNET_PROCESSINFO_H
#include <xnet/base/Types.h>
#include <xnet/base/Timestamp.h>
#include <xnet/base/StringPiece.h>

namespace xnet
{

namespace ProcessInfo
{
    pid_t       pid();
    string      pidString();
    uid_t       uid();
    string      username();
    uid_t       euid();
    Timestamp   startTime();
    int         clockTicksPerSecond;
    int         pageSize();
    bool        isDebugBuild();

    string      hostname();
    string      procname();
    StringPiece procname(const string& stat);

    string      procStatus();   //read /proc/self/status
    string      procStat();     //read /proc/self/stat
    string      threadStat();   //read /proc/self/task/tid/stat
    string      exePath();      //read /proc/self/exe
}

}

#endif //XNET_PROCESSINFO_H
