//
// Created by zhangkuo on 17-11-15.
//
#include <sys/resource.h>
#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <xnet/base/AsyncLogging.h>
#include <xnet/base/Logging.h>
#include <xnet/base/Timestamp.h>
using namespace xnet;

#define ROLL_SIZE 50*1000*1000
#define BATCH_NUM 10000

xnet::AsyncLogging* g_asyncLog = nullptr;

void asyncOutput(const char* logline, size_t len)
{
    g_asyncLog->append(logline, len);
}

void bench(bool isLongLog)
{
    xnet::Logger::setOutput(asyncOutput);

    std::string longStr(3000, 'K');
    longStr += " ";
    std::string emptyStr = " ";
    long long count = 0;

    for(int t = 0; t < 30; ++t)
    {
        auto start = xnet::Timestamp::now();
        for(int i=0; i<BATCH_NUM; i++)
        {
            LOG_INFO << "I love cpp! " << "0123456789 " << "abcdefghijklmnopqrstuvwxyz "
                     << (isLongLog ? longStr : emptyStr)
                     << count;
            ++count;
        }

        auto end = xnet::Timestamp::now();
        printf("%f ms\n", timeDifference(end, start)*1000*1000 / BATCH_NUM);

        sleep(1);
    }
}


int main(int argc, char* argv[])
{
    {
        // set max virtual memory to 2GB
        constexpr size_t kOneGB = 1000*1024*1024;
        rlimit rl = { 2*kOneGB, 2*kOneGB };
        setrlimit(RLIMIT_AS, &rl);
    }

    printf("pid = %d\n", getpid());

    char name[256];
    strncpy(name, argv[0], 256);

    xnet::AsyncLogging log(::basename(name), ROLL_SIZE);
    log.start();
    g_asyncLog = &log;

    bool isLongLog = true;
    bench(isLongLog);

    getchar();
    return 0;
}