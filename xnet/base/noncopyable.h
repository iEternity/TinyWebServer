//
// Created by zhangkuo on 17-10-22.
//

#ifndef XNET_NONCOPYABLE_H
#define XNET_NONCOPYABLE_H

namespace xnet
{

class noncopyable
{
protected:
    noncopyable() = default;
    virtual ~noncopyable() = default;
private:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

}

#endif //XNET_NONCOPYABLE_H
