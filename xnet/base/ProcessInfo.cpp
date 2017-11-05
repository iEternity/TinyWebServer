//
// Created by zhangkuo on 17-10-26.
//
#include <xnet/base/ProcessInfo.h>
#include <unistd.h>
#include <pwd.h>
#include <xnet/base/FileUtil.h>
#include <xnet/base/CurrentThread.h>

namespace xnet
{
namespace detail
{

Timestamp   g_startTime     = Timestamp::now();
int         g_clockTicks    = static_cast<int>(::sysconf(_SC_CLK_TCK));
int         g_pageSize      = static_cast<int>(::sysconf(_SC_PAGE_SIZE));

}
}

using namespace xnet;
using namespace xnet::detail;

pid_t ProcessInfo::pid()
{
    return ::getpid();
}

string ProcessInfo::pidString()
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", pid());
    return buf;
}

uid_t ProcessInfo::uid()
{
    return ::getuid();
}

string ProcessInfo::username()
{
    struct passwd pwd;
    struct passwd* result = nullptr;
    char buf[8*1024];
    const char* username = "unknown user";

    ::getpwuid_r(uid(), &pwd, buf, sizeof(buf), &result);
    if(result)
    {
        username = pwd.pw_name;
    }

    return username;
}

uid_t ProcessInfo::euid()
{
    return ::geteuid();
}

Timestamp ProcessInfo::startTime()
{
    return g_startTime;
}

int ProcessInfo::clockTicksPerSecond()
{
    return g_clockTicks;
}

int ProcessInfo::pageSize()
{
    return g_pageSize;
}

bool ProcessInfo::isDebugBuild()
{
#ifdef NDEBUG
    return false;
#else
    return true;
#endif
}

string ProcessInfo::hostname()
{
    char buf[256];
    if(::gethostname(buf, sizeof buf) == 0)
    {
        buf[255] = '\0';
        return buf;
    }
    else
    {
        return "unknown host";
    }
}

string ProcessInfo::procname()
{
    return procname(procStat()).asString();
}

StringPiece ProcessInfo::procname(const string& stat)
{
    StringPiece name;
    size_t left = stat.find('(');
    size_t right = stat.rfind(')');

    if(left != string::npos && right != string::npos && left < right)
    {
        name.set(stat.data() + left + 1, right - left - 1);
    }

    return name;
}

string ProcessInfo::procStatus()
{
    string result;
    FileUtil::readFile("/proc/self/status", 64*1024, &result);
    return result;
}

string ProcessInfo::procStat()
{
    string result;
    FileUtil::readFile("proc/self/stat", 64*1024, &result);
    return result;
}

string ProcessInfo::threadStat()
{
    char buf[64];
    snprintf(buf, sizeof(buf), "/proc/self/task/%d/stat", CurrentThread::tid());

    string result;
    FileUtil::readFile(buf, 64*1024, &result);
    return result;
}

string ProcessInfo::exePath()
{
    string result;
    char buf[1024];
    ssize_t n = ::readlink("proc/self/exe", buf, sizeof(buf));
    if(n > 0)
    {
        result.assign(buf, n);
    }
    return result;
}