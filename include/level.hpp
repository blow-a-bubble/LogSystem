#pragma once
/*日志等级
    UNKNOWN  未知日志
    DEBUG    调试日志
    INFO     信息日志
    WARNING  警告日志
    ERROR    错误日志
    FATAL    崩溃错误日志
    OFF      关闭日志
*/
#include <iostream>
#include <string>

namespace logSys
{
    class LogLevel
    {
    public:
        enum class Level
        {
            UNKNOWN,
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            FATAL,
            OFF
        };
        //日志等级转字符串
        static std::string toString(Level level)
        {
            switch (level)
            {
            case Level::DEBUG:
                return "DEBUG";
                break;
            case Level::INFO:
                return "INFO";
                break;
            case Level::WARNING:
                return "WARNING";
                break;
            case Level::ERROR:
                return "ERROR";
                break;
            case Level::FATAL:
                return "FATAL";
                break;
            }
            return "UNKNOWN";
        }
    };
}

