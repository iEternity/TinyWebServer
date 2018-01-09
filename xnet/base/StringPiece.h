//
// Created by zhangkuo on 17-9-16.
//

#ifndef XNET_STRINGPIECE_H
#define XNET_STRINGPIECE_H
#include <string>
#include <cstring>
#include <iostream>

namespace xnet
{

class StringArg
{
public:
    StringArg(const char* str):
            str_(str)
    {
    }

    StringArg(const std::string& str):
            str_(str.data())
    {
    }

    const char* c_str() const
    {
        return str_;
    }
private:
    const char* str_;
};

class StringPiece
{
private:
    const char* ptr_;
    size_t length_;
public:
    StringPiece():ptr_(nullptr), length_(0){}
    StringPiece(const char* str):
            ptr_(str),
            length_(strlen(str))
    {
    }
    StringPiece(const std::string& str):
            ptr_(str.data()),
            length_(str.size())
    {
    }
    StringPiece(const char* offset, size_t len):
            ptr_(offset),
            length_(len)
    {
    }

    const char* data() const { return ptr_; }
    size_t size() const { return length_; }
    bool empty() const { return length_ == 0; }
    const char* begin() { return ptr_ ;}
    const char* end() { return ptr_ + length_; }
    void clear()
    {
        ptr_ = nullptr;
        length_ = 0;
    }
    void set(const char* str)
    {
        ptr_ = str;
        length_ = strlen(str);
    }
    void set(const char* buffer, size_t len)
    {
        ptr_= buffer;
        length_ = len;
    }
    void set(const void* buffer, size_t len)
    {
        ptr_ = reinterpret_cast<const char*>(buffer);
        length_ = len;
    }
    char operator[] (int i) const
    {
        return ptr_[i];
    }
    void removePrefix(size_t n)
    {
        ptr_ += n;
        length_ -= n;
    }
    void removeSuffix(size_t n)
    {
        length_ -= n;
    }

    bool operator== (const StringPiece& rhs) const
    {
        return (length_ == rhs.length_) && (memcmp(ptr_, rhs.ptr_, length_) == 0);
    }
    bool operator!= (const StringPiece& rhs) const
    {
        return !((*this) == rhs);
    }

    int compare(const StringPiece& rhs) const
    {
        int ret = memcmp(ptr_, rhs.ptr_, length_ < rhs.length_ ? length_ : rhs.length_);
        if(ret == 0)
        {
            if(length_ < rhs.length_) ret = -1;
            else if(length_ > rhs.length_) ret = 1;
        }
        return ret;
    }

    std::string asString() const
    {
        return std::string(data(), size());
    }

    void copyToString(std::string& target) const
    {
        target.assign(ptr_, size());
    }

    bool startsWith(const StringPiece& rhs) const
    {
        return (length_ >= rhs.length_) && (memcmp(rhs.ptr_, ptr_, rhs.length_) == 0);
    }
};

std::ostream& operator<< (std::ostream& o,  const StringPiece& str);

}

#endif //XNET_STRINGPIECE_H
