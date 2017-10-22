//
// Created by zhangkuo on 17-10-22.
//
#include <xnet/base/FileUtil.h>
#include <xnet/base/Logging.h>
#include <iostream>
#include <xnet/base/Utility.h>

using namespace xnet;

int main()
{
    std::string result;
    std::string filePath = "/home/zhangkuo/CoolCode/xNet/";
    int64_t fileSize = 0;
    int64_t modifyTime = 0;
    int64_t createTime = 0;

    int errorNo = FileUtil::readFile(filePath, 1024, &result, &fileSize, &modifyTime, &createTime);
    cout << "result: " << result << endl;
    cout << "filePath: " << filePath << endl;
    cout << "modifyTime: " << utility::formatTime(modifyTime) << endl;
    cout << "createTime: " << utility::formatTime(createTime) << endl;
    cout << "error: " << utility::errnoToString(errorNo) << " (errno: " << errno << ")" << endl;

    return 0;
}