//
// Created by zhangkuo on 17-8-19.
//

#ifndef XNET_ATOMIC_H
#define XNET_ATOMIC_H
#include <boost/noncopyable.hpp>
#include <atomic>

namespace xnet
{
namespace detail
{
template<typename T>
class AtomicIntegerT : boost::noncopyable
{
public:
    AtomicIntegerT()
            : value_(0)
    {

    }

    T get()
    {
        return __atomic_load_n(&value_, __ATOMIC_SEQ_CST);
    }

    T getAndAdd(T x)
    {
        return __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST);
    }

    T addAndGet(T x)
    {
        return getAndAdd(x) + x;
    }

    T incrementAndGet()
    {
        return addAndGet(1);
    }

    T decrementAndGet()
    {
        return addAndGet(-1);
    }

    void add(T x)
    {
        getAndAdd(x);
    }

    void increment()
    {
        incrementAndGet();
    }

    void decrement()
    {
        incrementAndGet();
    }

    T getAndSet(T newValue)
    {
        return __atomic_store_n(&value_, newValue, __ATOMIC_SEQ_CST);
    }

private:
    volatile T value_;
};

}   //namespace detail

    using AtomicInt32 = detail::AtomicIntegerT<int32_t>;
    using AtomicInt64 = detail::AtomicIntegerT<int64_t>;

}   //namespace xNet

#endif //XNET_ATOMIC_H