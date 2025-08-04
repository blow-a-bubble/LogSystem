#pragma once
#include "util.hpp"
#include <fstream>
#include <sstream>
#include <memory>
#include <cassert>
#include <iomanip>
/*
    日志落地类
        1. 标准输出
        2. 文件
        3. 滚动文件
*/
namespace logSys
{
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        virtual ~LogSink() = default;
        virtual void log(const char* data, size_t len) = 0;
    };
    // 标准输出日志落地类
    class StdoutSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<StdoutSink>;
        void log(const char* data, size_t len) override
        {
            std::cout.write(data, len);
        }
    };
    // 文件日志落地类
    class FileSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<FileSink>;
        FileSink(const std::string &pathname)
        :_pathname(pathname)
        {
            // 1.创建目录
            util::File::createDirectory(util::File::path(_pathname));
            // 2.创建文件句柄
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            assert(_ofs.good());
        }
        void log(const char* data, size_t len) override
        {
            _ofs.write(data, len);
            if(!_ofs.good())
            {
                std::cout << "write to file failed!" << std::endl;
            }
        }
    private:
        std::string _pathname;
        std::ofstream _ofs; // 文件句柄，避免多次打开关闭
    };
    // 大小滚动文件日志落地类
    class RollBySizeSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<RollBySizeSink>;
        RollBySizeSink(const std::string &basename, size_t max_size)
        :_basename(basename), _max_size(max_size), _cur_size(0)
        {
            util::File::createDirectory(util::File::path(_basename));  
        }
        void log(const char* data, size_t len) override
        {
            initLogFile();
            _ofs.write(data, len);
            if(!_ofs.good())
            {
                std::cout << "write to rollfile failed\n"; 
                return;
            }
            _cur_size += len;
        }
    private:
        // 文件未打开或写到最大值进行文件滚动
        void initLogFile()
        {
            if(!_ofs.is_open() || _cur_size >= _max_size)
            {
                _ofs.close();
                std::string pathname = createNewFile();
                _ofs.open(pathname, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
                _cur_size = 0;
            }
        }
        // 根据时间和计数器创建文件名
        std::string createNewFile()
        {
            time_t t = util::Date::now();
            struct std::tm tl;
            localtime_r(&t, &tl);
            char buffer[64] = { 0 };
            strftime(buffer, 63, "%Y-%m-%d %H:%M:%S", &tl);
            std::string pathname = _basename + buffer + "-" + std::to_string(_count++);
            pathname += ".log";  
            return pathname;
        }
    private:
        std::string _basename;
        std::ofstream _ofs;
        size_t _max_size; 
        size_t _cur_size; // 当前文件大小，避免重复获取
        int _count; // 文件计数，避免同一时刻滚动多个文件导致文件名一样
    };

    // 日志落地工厂模式
    class SinkFactory
    {
    public:
        template<typename SinkType, typename ...Args>
        static LogSink::ptr create(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };
}


