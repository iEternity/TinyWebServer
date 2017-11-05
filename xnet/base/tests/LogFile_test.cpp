//
// Created by zhangkuo on 17-11-4.
//
#include <xnet/base/LogFile.h>
#include <xnet/base/Logging.h>

using namespace xnet;

std::shared_ptr<LogFile> g_logFile;

void outputFunc(const char* msg, int n)
{
    g_logFile->append(msg, n);
}

void outputFlush()
{
    g_logFile->flush();
}

int main(int argc, char* argv[])
{
    char name [256];
    strncpy(name, argv[0], 256);
    g_logFile.reset(new LogFile(::basename(name), 1024*1024));
    xnet::Logger::setOutput(outputFunc);
    xnet::Logger::setFlush(outputFlush);

    string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    for(int i=0; i<100000; i++)
    {
        LOG_INFO << line;
        usleep(100);
    }
}