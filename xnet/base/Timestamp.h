//
// Created by zhangkuo on 17-8-7.
//

#ifndef XNET_TIMESTAMP_H
#define XNET_TIMESTAMP_H

#include <boost/operators.hpp>
#include <string>

namespace xnet
{
class Timestamp : boost::less_than_comparable<Timestamp>
{
public:
    Timestamp()
        : microsecondsSinceEpoch_(0)
    {
    }

    explicit Timestamp(int64_t microSecondsSinceEpochArg)
        : microsecondsSinceEpoch_(microSecondsSinceEpochArg)
    {
    }

    void swap(Timestamp& that)
    {
        std::swap(microsecondsSinceEpoch_, that.microsecondsSinceEpoch_);
    }

    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds)const;

    bool valid() { return microsecondsSinceEpoch_ > 0; }

    int64_t microsecondsSinceEpoch() const { return microsecondsSinceEpoch_; }
    time_t secondsSinceEpoch() const
    {
        return static_cast<time_t>(microsecondsSinceEpoch_ / kMicrosecondsPerSecond);
    }

    static Timestamp now();
    static Timestamp invalid()
    {
        return Timestamp();
    }

    static Timestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t, 0);
    }

    static Timestamp fromUnixTime(time_t t, int microseconds)
    {
        return Timestamp(static_cast<int64_t>(t) * kMicrosecondsPerSecond + microseconds);
    }

    static const int kMicrosecondsPerSecond = 1000 * 1000;
private:
    int64_t microsecondsSinceEpoch_;
};

inline bool operator<(const Timestamp& lhs,const Timestamp& rhs)
{
    return lhs.microsecondsSinceEpoch() < rhs.microsecondsSinceEpoch();
}

inline bool operator == (const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.microsecondsSinceEpoch() == rhs.microsecondsSinceEpoch();
}

inline double timeDifference(const Timestamp& high, const Timestamp& low)
{
    int64_t diff = high.microsecondsSinceEpoch() - low.microsecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicrosecondsPerSecond;
}

inline Timestamp addTime(const Timestamp& timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicrosecondsPerSecond);
    return Timestamp(timestamp.microsecondsSinceEpoch() + delta);
}

}
#endif //XNET_TIMESTAMP_H
