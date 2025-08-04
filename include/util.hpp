#pragma once
/*日志工具类
    1. 获取当前时间戳
    2. 判断文件或目录是否存在
    3. 获取文件所在目录
    4. 创建目录
*/
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <ctime>
namespace logSys
{
    namespace util
    {
        class Date
        {
        public:
            static time_t now()
            {
                return time(nullptr);
            }
        };

        class File
        {
        public:
            static bool exists(const std::string &pathname)
            {
                struct stat statbuf;
                return stat(pathname.c_str(), &statbuf) == 0;
            }

            static std::string path(const std::string &pathname)
            {
                // ./abc/def/a.txt -> ./abc/def/
                auto pos = pathname.find_last_of("/\\");
                if(pos == std::string::npos) return "./";
                else return pathname.substr(0, pos + 1);
            }

            static void createDirectory(const std::string &pathname)
            {
                if(pathname.empty()) return;
                if(exists(pathname)) return;
                // /abc/def/hij/
                size_t index = 0, pos = 0;
                while(index < pathname.size())
                {
                    pos = pathname.find_first_of("/\\", index);
                    if(pos == index)
                    {
                        index = pos + 1;
                        continue;
                    }
                    if(pos == std::string::npos) 
                    {
                        mkdir(pathname.c_str(), 0777);
                        return;
                    }
                    auto dir = pathname.substr(0, pos + 1);
                    if(exists(dir))
                    {
                        index = pos + 1;
                        continue;
                    }
                    else
                    {
                        mkdir(dir.c_str(), 0777);
                        index = pos + 1;
                        continue;
                    }
                }
                
            }
        };
    }
}