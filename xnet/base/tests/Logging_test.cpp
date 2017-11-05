//
// Created by zhangkuo on 17-10-13.
//
#include <xnet/base/Logging.h>
#include <unistd.h>
#include <xnet/base/ThreadPool.h>
#include <xnet/base/LogFile.h>

using namespace xnet;

int64_t g_total;
FILE* g_file;
std::unique_ptr<LogFile> g_logFile;

void dummyOutput(const char* msg, int len)
{
    g_total += len;
    if(g_file)
    {
        fwrite(msg, 1, len, g_file);
    }
    else if(g_logFile)
    {
        g_logFile->append(msg, len);
    }
}

void bench(const char* type)
{
    xnet::Logger::setOutput(dummyOutput);
    Timestamp start(Timestamp::now());
    g_total = 0;

    int n = 1000*1000;
    string longStr(3000, 'X');
    longStr += " ";
    for(int i=0; i<n; i++)
    {
        LOG_INFO << "Hello 0123456789 " << "abecefghijklmnopqrstuvwxyz "
                 << longStr
                 << i;
    }

    Timestamp end(Timestamp::now());
    double seconds = timeDifference(end, start);
    printf("%12s: %f seconds, %lld bytes, %10.2f msg/s, %.2f MiB/s\n",
            type, seconds, g_total, n/seconds, g_total/seconds/(1024*1024));
}

void logInThread()
{
    LOG_INFO << "logInThread";
    usleep(1000);
}

int main()
{
    getpid();

    ThreadPool pool("pool");
    pool.start(5);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);

    LOG_TRACE   << "trace";
    LOG_DEBUG   << "debug";
    LOG_INFO    << "This is info";
    LOG_WARN    << "This is warn";
    LOG_ERROR   << "This is error";

    LOG_INFO    << "sizeof LogLevel: "      << sizeof(Logger::LogLevel);
    LOG_INFO    << "sizeof SourceFile: "    << sizeof(Logger::SourceFile);
    LOG_INFO    << "sizeof Logger: "        << sizeof(Logger);
    LOG_INFO    << "sizeof LogStream: "     << sizeof(LogStream);
    LOG_INFO    << "sizeof Buffer: "        << sizeof(LogStream::Buffer);

    sleep(1);
    bench("nop");

    char buffer[64*1024];
    g_file = fopen("/dev/null", "w");
    ::setbuffer(g_file, buffer, sizeof(buffer));
    bench("/dev/null");
    fclose(g_file);

    g_file = fopen("/tmp/log", "w");
    ::setbuffer(g_file, buffer, sizeof buffer);
    bench("/tmp/log");
    fclose(g_file);

    g_file = nullptr;
    g_logFile.reset(new LogFile("test_log_st", 500*1000*1000, false));
    bench("test_log_st");

    g_logFile.reset(new LogFile("test_log_mt", 500*1000*1000, true));
    bench("test_log_mt");
    g_logFile.reset();
}