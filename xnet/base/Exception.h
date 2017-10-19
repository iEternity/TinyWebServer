//
// Created by zhangkuo on 17-10-19.
//

#ifndef XNET_EXCEPTION_H
#define XNET_EXCEPTION_H
#include <exception>
#include "Types.h"

namespace xnet
{

class Exception : public std::exception
{
public:
    explicit Exception(const char*);
    explicit Exception(const string& );

    virtual ~Exception() throw() = default;
    virtual const char* what() const throw();
    const char* stackTrace() const throw();

private:
    void fillStackStrace();

    string message_;
    string stack_;
};

}

#endif //XNET_EXCEPTION_H
