#pragma once
#include "util.hpp"
#include "level.hpp"
#include "formatter.hpp"
#include "sink.hpp"
#include "message.hpp"
#include "looper.hpp"
#include <cstdarg>
#include <mutex>
#include <atomic>
#include <unordered_map>
// 抽象日志器类
namespace logSys
{
    
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string &logger_name,
               LogLevel::Level limit_level,
               const std::shared_ptr<Formatter> &formatter,
               std::vector<LogSink::ptr> sinks)
            : _logger_name(logger_name),
              _limit_level(limit_level), _formatter(formatter),
              _sinks(sinks.begin(), sinks.end())
        {}
        virtual ~Logger() = default;
        std::string getName() const{ return _logger_name; };
        void debug(const char *file, size_t line, const char *fmt, ...)
        {
            if (LogLevel::Level::DEBUG < _limit_level)
                return;
            va_list al;
            va_start(al, fmt);
            std::string msg = serialize(LogLevel::Level::DEBUG, file, line, fmt, al);
            va_end(al);
            log(msg);
        }
        void info(const char *file, size_t line, const char *fmt, ...)
        {
            if (LogLevel::Level::INFO < _limit_level)
                return;
            va_list al;
            va_start(al, fmt);
            std::string msg = serialize(LogLevel::Level::INFO, file, line, fmt, al);
            va_end(al);
            log(msg);
        }
        void warn(const char *file, size_t line, const char *fmt, ...)
        {
            if (LogLevel::Level::WARNING < _limit_level)
                return;
            va_list al;
            va_start(al, fmt);
            std::string msg = serialize(LogLevel::Level::WARNING, file, line, fmt, al);
            va_end(al);
            log(msg);
        }
        void error(const char *file, size_t line, const char *fmt, ...)
        {
            if (LogLevel::Level::ERROR < _limit_level)
                return;
            va_list al;
            va_start(al, fmt);
            std::string msg = serialize(LogLevel::Level::ERROR, file, line, fmt, al);
            va_end(al);
            log(msg);
        }
        void fatal(const char *file, size_t line, const char *fmt, ...)
        {
            if (LogLevel::Level::FATAL < _limit_level)
                return;
            va_list al;
            va_start(al, fmt);
            std::string msg = serialize(LogLevel::Level::FATAL, file, line, fmt, al);
            va_end(al);
            log(msg);
        }

    protected:
        // 
        std::string serialize(LogLevel::Level level, const char *file, size_t line, const char *fmt, va_list al)
        {
            char *buffer = nullptr;
            std::string str;
            if (vasprintf(&buffer, fmt, al) < 0)
            {
                std::cout << "格式化字符串失败" << std::endl;
            }
            str += buffer;
            free(buffer);
            LogMsg lm(level, line, file, _logger_name, str);
            std::string msg = _formatter->format(lm);
            return msg;
        }
        // 抽象实际落地方式
        virtual void log(const std::string &msg) = 0;

    protected:
        std::string _logger_name;         // 日志器名称
        std::atomic<LogLevel::Level> _limit_level;     // 日志器输出等级限制
        std::shared_ptr<Formatter> _formatter;             // 日志格式化器
        std::vector<LogSink::ptr> _sinks; // 多个日志落地方式
        std::mutex _mutex;                // 锁，避免多个线程使用一个日志器冲突
    };

    // 同步日志器
    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string &logger_name,
                   LogLevel::Level limit_level,
                   const std::shared_ptr<Formatter> &formatter,
                   std::vector<LogSink::ptr> sinks)
            : Logger(logger_name, limit_level, formatter, sinks)
        {
        }
    protected:
        void log(const std::string &msg) override
        {
            std::lock_guard<std::mutex> lock(_mutex);
            for (auto &sink : _sinks)
            {
                sink->log(msg.c_str(), msg.length());
            }
        }
    };
    // 异步日志器
    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(const std::string &logger_name,
                    LogLevel::Level limit_level,
                    const std::shared_ptr<Formatter> &formatter,
                    std::vector<LogSink::ptr> sinks,
                    AsyncType async_type = AsyncType::AsyncSafe)
            : Logger(logger_name, limit_level, formatter, sinks),
            _looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::asyncLog, this, std::placeholders::_1), async_type))
        {
        }
    protected:
        void log(const std::string &msg) override
        {
            _looper->push(msg);
        }
        // 实际异步线程的落地回调
        void asyncLog(Buffer &buffer)
        {
            // 不用加锁因为只有一个异步线程
            for(auto &sink : _sinks)
            {
                sink->log(buffer.readPositon(), buffer.readAbleSize());
            }
        } 
    private:  
        // 异步工作器
        AsyncLooper::ptr _looper;
    };

    // 枚举日志器类型
    enum class LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC
    };
    // 抽象日志器建造者模式, 由于构造函数参数没有顺序要求，为了简便取消指挥者
    // 派送出全局日志器和局部日志器建造者
    class LoggerBuilder
    {
    public:
        LoggerBuilder()
        :_logger_type(LoggerType::LOGGER_SYNC), _limit_level(LogLevel::Level::DEBUG),
        _async_type(AsyncType::AsyncSafe)
        {
        }
        using ptr = std::shared_ptr<LoggerBuilder>;
        void buildLoggerName(const std::string &logger_name) { _logger_name = logger_name; }
        void buildLoggerType(LoggerType logger_type) { _logger_type = logger_type; }
        void buildLimitLevel(LogLevel::Level limit_level) { _limit_level = limit_level; }
        void buildFormatter(const Formatter::ptr &formatter) { _formatter = formatter; }
        void buildFormatter(const std::string &pattern) { _formatter = std::make_shared<Formatter>(pattern); }
        void buildAsyncType(AsyncType async_type) { _async_type = async_type; }
        template<typename SinkType, typename ...Args>
        void buildSink(Args &&...args)
        {
            _sinks.push_back(SinkFactory::create<SinkType>(std::forward<Args>(args)...));
        }
        // 抽象建造日志器类
        virtual Logger::ptr build() = 0;
    protected:
        std::string _logger_name;         // 日志器名称
        LoggerType _logger_type;          // 日志器类型(同步/异步)
        LogLevel::Level _limit_level;     // 日志器输出等级限制
        Formatter::ptr _formatter;        // 日志格式化器
        std::vector<LogSink::ptr> _sinks; // 多个日志落地方式
        AsyncType _async_type;            // 异步缓冲区类型
    };

    // 局部日志器建造者
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override 
        {
            if(_logger_name.empty()) 
            {
                std::cout << "没有设置日志器名字" << std::endl;
                abort();
            }
            if(_formatter.get() == nullptr)
            {
                std::cout << "没有设置日志格式化模式, 设置默认模式: " << "[ %d{%H:%M:%S}%T%t%T[%p]%T[%c]%T%f:%l%T%m%n ]" << std::endl;
                _formatter = std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
                std::cout << "没有设置日志落地方向， 设置默认标准输出" << std::endl; 
                _sinks.push_back(SinkFactory::create<StdoutSink>());
            }
            
            if(_logger_type == LoggerType::LOGGER_ASYNC)
            {
                return std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _async_type); 
            }
            else
            {
                return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
            } 
        }
    };
    // 全局单例日志管理器
    class LoggerManager
    {
    public:
        static LoggerManager& getInstance()
        {
            static LoggerManager _instance;
            return _instance;
        }
        void addLogger(const Logger::ptr &logger)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _loggers.emplace(logger->getName(), logger);
        }
        bool hasLogger(const std::string &name)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _loggers.find(name) != _loggers.end();
        }
        Logger::ptr getLogger(const std::string &name)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if(_loggers.find(name) == _loggers.end()) return nullptr;
            return _loggers[name];
        }
        Logger::ptr rootLogger()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _root_logger;
        }
    private:
        LoggerManager()
        {
            LoggerBuilder::ptr builder = std::make_shared<LocalLoggerBuilder>();
            builder->buildLoggerName("root");
            _root_logger = builder->build();
            addLogger(_root_logger);
        }
    private:
        std::mutex _mutex; // unordered_map线程不安全，curd都得加锁
        std::unordered_map<std::string, Logger::ptr> _loggers; 
        Logger::ptr _root_logger; // 默认的日志器: 输出到显示器
    };

    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            if(_logger_name.empty())
            {
                std::cout << "没有设置日志器名字" << std::endl;
                abort();
            }
            if(_formatter.get() == nullptr)
            {
                std::cout << "没有设置日志格式化模式, 设置默认模式: " << "[ %d{%H:%M:%S}%T%t%T[%p]%T[%c]%T%f:%l%T%m%n ]" << std::endl;
                _formatter = std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
                std::cout << "没有设置日志落地方向， 设置默认标准输出" << std::endl; 
                _sinks.push_back(SinkFactory::create<StdoutSink>());
            }
            Logger::ptr ret;
            if(_logger_type == LoggerType::LOGGER_SYNC)
            {
                ret = std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
            }
            else
            {
                ret = std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _async_type);
            }
            LoggerManager::getInstance().addLogger(ret);
            return ret;
        }
    };
}
