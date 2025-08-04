#pragma once
/*日志消息类
*/
#include "level.hpp"
#include "util.hpp"
#include <iostream>
#include <string>
#include <thread>

namespace logSys
{
    struct LogMsg
    {
        time_t _ctime;              // 日志创建时间戳
        LogLevel::Level _level;     // 日志等级
        std::thread::id _pid;       // 日志线程id
        size_t _line;               // 日志行号
        std::string _file;          // 日志所在文件
        std::string _name;        // 日志器名称
        std::string _payload;       // 日志信息
        LogMsg(LogLevel::Level level, size_t line, 
               const std::string &file,
               const std::string &name,
               const std::string &payload)
        :_ctime(util::Date::now())
        ,_level(level)
        ,_pid(std::this_thread::get_id())
        ,_line(line)
        ,_file(file)
        ,_name(name)
        ,_payload(payload)
        {}
    };
}