//
// Created by zhangkuo on 17-10-22.
//

#ifndef XNET_FILEUTIL_H
#define XNET_FILEUTIL_H
#include <xnet/base/noncopyable.h>
#include <xnet/base/StringPiece.h>
#include <xnet/base/Types.h>

namespace xnet
{
namespace FileUtil
{

//read small file < 64KB
class ReadSmallFile : noncopyable
{
public:
    explicit ReadSmallFile(const string& fileName);
    ~ReadSmallFile();

    int readToString(int maxSize, string* content, int64_t* fileSize, int64_t* modifyTime, int64_t* createTime);
    int readToBuffer(int* size);

    const char* buffer() const { return buffer_; }

private:
    static const int kBufferSize = 64*1024;

    int fd_;
    int errno_;
    char buffer_[kBufferSize];
};

inline int readFile(const string& fileName, int maxSize, string* content,
                    int64_t* fileSize = nullptr, int64_t* modifyTime = nullptr, int64_t* createTime = nullptr)
{
    ReadSmallFile file(fileName);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

}

class AppendFile : noncopyable
{
public:
    explicit AppendFile(const string& fileName);
    ~AppendFile();

    void append(const char* logLine, const size_t len);
    void flush();
private:
    static const int kBufferSize = 64*1024;

    FILE* fp_;
    char buffer_[kBufferSize];
    size_t writenBytes_;
};

}

#endif //XNET_FILEUTIL_H
