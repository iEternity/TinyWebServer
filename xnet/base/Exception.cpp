//
// Created by zhangkuo on 17-10-19.
//
#include "Exception.h"
#include <execinfo.h>

using namespace xnet;

Exception::Exception(const char* msg):
        message_(msg)
{
    fillStackStrace();
}

Exception::Exception(const string& msg):
        message_(msg)
{
    fillStackStrace();
}

const char* Exception::what() const throw()
{
    return message_.data();
}

const char* Exception::stackTrace() const throw()
{
    return stack_.data();
}

void Exception::fillStackStrace()
{
    const int len = 200;
    void* buffer[len];
    int nptrs = ::backtrace(buffer, len);
    char** strings = ::backtrace_symbols(buffer, nptrs);

    if(strings)
    {
        for(int i=0; i<nptrs; i++)
        {
            stack_.append(strings[i]);
            stack_.push_back('\n');
        }
        free(strings);
    }
}