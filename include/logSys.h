#pragma once
#include "logger.hpp"
namespace logSys
{
    Logger::ptr getLogger(const std::string &name)
    {
        return LoggerManager::getInstance().getLogger(name);
    }
    Logger::ptr rootLogger()
    {
        return LoggerManager::getInstance().rootLogger();
    }
    // 日志系统全局接口
    // 用宏函数实现代理模式代理接口
    #define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, #__VA_ARGS__)
    #define info(fmt, ...) info(__FILE__, __LINE__, fmt, #__VA_ARGS__)
    #define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, #__VA_ARGS__)
    #define error(fmt, ...) error(__FILE__, __LINE__, fmt, #__VA_ARGS__)
    #define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, #__VA_ARGS__)

    #define LOGDEBUG(fmt, ...) rootLogger()->debug(fmt, #__VA_ARGS__)
    #define LOGINFO(fmt, ...) rootLogger()->info(fmt, #__VA_ARGS__)
    #define LOGWARN(fmt, ...) rootLogger()->warn(fmt, #__VA_ARGS__)
    #define LOGERROR(fmt, ...) rootLogger()->error(fmt, #__VA_ARGS__)
    #define LOGFATAL(fmt, ...) rootLogger()->fatal(fmt, #__VA_ARGS__)
}