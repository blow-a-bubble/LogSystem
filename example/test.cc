#include "logSys.h"
#include <iostream>
#include <unistd.h>
/*
    日志落地方式扩展: 以时间段方式落地
*/
// 通过枚举时间段，避免让用户自己计算
enum class Time_Gap
{
    SECOND_GAP,
    MINUTE_GAP,
    HOUR_GAP,
    DAY_GAP
};
class RollByTimeSink : public logSys::LogSink
{
public:
    RollByTimeSink(const std::string &basename, Time_Gap gap_type)
    :_basename(basename)
    {
        switch(gap_type)
        {
            case Time_Gap::SECOND_GAP: _time_gap = 1; break;
            case Time_Gap::MINUTE_GAP: _time_gap = 60; break;
            case Time_Gap::HOUR_GAP: _time_gap = 60 * 60; break;
            case Time_Gap::DAY_GAP: _time_gap = 60 * 60 * 24; break;
        }
        time_t tm = logSys::util::Date::now();
        // 处理时间间隔为1的特殊情况
        _cur_gap = _time_gap == 1 ? tm : tm % _time_gap;
        std::string pathname = createNewFile();
        logSys::util::File::createDirectory(logSys::util::File::path(pathname));
        _ofs.open(pathname, std::ios::binary | std::ios::app);
        assert(_ofs.good());
    }
    void log(const char *data, size_t len) override
    {
        time_t tm = logSys::util::Date::now();
        size_t cur_gap = _time_gap == 1 ? tm : tm %  _time_gap;
        if(cur_gap != _cur_gap)
        {
            _cur_gap = cur_gap;
            _ofs.close(); // 先关闭之前文件
            std::string pathname = createNewFile();
            _ofs.open(pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        _ofs.write(data, len);
    }
private:
    std::string createNewFile()
    {
        time_t t = logSys::util::Date::now();
        struct std::tm tl;
        localtime_r(&t, &tl);
        char buffer[64] = { 0 };
        strftime(buffer, 63, "%Y-%m-%d %H:%M:%S", &tl);
        std::string pathname = _basename + buffer;
        pathname += ".log";
        return pathname;
    }
private:
    std::string _basename;
    std::ofstream _ofs;
    size_t _time_gap;
    size_t _cur_gap;
};
void test_logger()
{
    auto logger = logSys::LoggerManager::getInstance().getLogger("root");
    int count = 0;
    while(count < 500000)
    {
        logger->error(__FILE__, __LINE__, "mylog: %d", count++);
    }
}
int main()
{
    logSys::LOGDEBUG("%s", "测试日志");
    logSys::LOGINFO("%s", "测试日志");
    logSys::LOGWARN("%s", "测试日志");
    logSys::LOGERROR("%s", "测试日志");
    logSys::LOGFATAL("%s", "测试日志");
    
/*
    logSys::LoggerBuilder::ptr lp = std::make_shared<logSys::GlobalLoggerBuilder>();
    lp->buildLoggerName("async_logger");
    lp->buildLoggerType(logSys::LoggerType::LOGGER_ASYNC);
    lp->buildSink<logSys::RollBySizeSink>("./logdir/rollbysize", 1024 * 1024);
    lp->build(); // 创建了全局日志器
    test_logger();
*/ 
/*
    // test for buffer
    // 1. read file
    std::ifstream ifs("/home/blow/github/LogSystem/build/logdir/name.log", std::ios::binary);
    if(ifs.is_open() == false)
    {
        std::cerr << "open file errer\n";
        return -1;
    }
    size_t fileSize = 0;
    ifs.seekg(0, std::ios::end);
    fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::string content;
    content.resize(fileSize);
    ifs.read(&content[0], fileSize);
    ifs.close();
    std::cout << "read size: " << fileSize << std::endl;
    // write to buffer
    logSys::Buffer buffer;
    for(int i = 0; i < content.size(); ++i)
    {
        buffer.writeAndPush(&content[i], 1);
    }
    std::cout << "buffer size: " << buffer.readAbleSize() << std::endl;
    // from buffer to another file
    std::ofstream ofs("/home/blow/github/LogSystem/build/logdir/temp.log", std::ios::binary);
    if(ofs.is_open() == false)
    {
        std::cerr << "open file errer\n";
        return -1;
    }
    size_t sz = buffer.readAbleSize();
    for (size_t i = 0; i < sz; i++)
    {
        ofs.write(buffer.readPositon(), 1);
        buffer.moveReadBack(1);
    }
    ofs.close();
*/
/*
    logSys::LoggerBuilder::ptr builder = std::make_shared<logSys::LocalLoggerBuilder>();
    builder->buildLoggerName("async-logger");
    builder->buildLimitLevel(logSys::LogLevel::Level::DEBUG);
    builder->buildLoggerType(logSys::LoggerType::LOGGER_ASYNC);
    builder->buildFormatter("[%c:%m] %n");
    builder->buildSink<logSys::FileSink>("./logdir/async.log");
    builder->buildSink<logSys::StdoutSink>();
    builder->buildAsyncType(logSys::AsyncType::AsyncUnSafe);
    logSys::Logger::ptr logger = builder->build();
    logger->fatal(__FILE__, __LINE__, "mylog: %s", "hello");
    logger->warn(__FILE__, __LINE__, "mylog: %s", "hello");
    int count = 0;
    while(count < 500000)
    {
        logger->error(__FILE__, __LINE__, "mylog: %d", count++);
        // usleep(1000);
    }
*/
/*
    logSys::LogMsg msg(logSys::LogLevel::Level::DEBUG, __LINE__, __FILE__, "mylog", "测试格式化功能...");
    std::shared_ptr<logSys::Formatter> f = std::make_shared<logSys::Formatter>("[%d{%H:%M:%S}]%T[%t]%T[%p]%T[%c]%T%f:%l%T%m%n");
    std::vector<logSys::LogSink::ptr> sinks;
    logSys::LogSink::ptr psink = logSys::SinkFactory::create<RollByTimeSink>("./logdir/mylog", Time_Gap::SECOND_GAP);
    sinks.push_back(psink);
    psink = logSys::SinkFactory::create<logSys::StdoutSink>();
    sinks.push_back(psink);
    psink = logSys::SinkFactory::create<logSys::FileSink>("./logdir/name.log");
    sinks.push_back(psink);
    logSys::Logger::ptr logger = std::make_shared<logSys::SyncLogger>("mylogger", logSys::LogLevel::Level::WARNING, f, sinks);
    
    time_t old = logSys::util::Date::now();
    while(logSys::util::Date::now() < old + 5)
    {
        logger->error(__FILE__, __LINE__, "mylog: %s", "hello");
        usleep(1000);
    }
*/
/*
    logSys::LogMsg msg(logSys::LogLevel::Level::DEBUG, __LINE__, __FILE__, "mylog", "测试格式化功能...");
    logSys::Formatter f("[%d{%H:%M:%S}]%T[%t]%T[%p]%T[%c]%T%f:%l%T%m%n");

    auto ret = f.format(msg);
    logSys::LogSink::ptr psink = logSys::SinkFactory::create<RollByTimeSink>("./logdir/mylog", Time_Gap::SECOND_GAP);
    time_t old = logSys::util::Date::now();
    while(logSys::util::Date::now() < old + 5)
    {
        psink->log(ret.c_str(), ret.size());
        usleep(1000);
    }
*/
    // std::cout << ret;
/*
    logSys::LogSink::ptr psink = logSys::SinkFactory::create<logSys::StdoutSink>();;
    psink->log(ret.c_str(), ret.length());

    psink = logSys::SinkFactory::create<logSys::FileSink>("./logdir/name.log");
    psink->log(ret.c_str(), ret.length());

    psink = logSys::SinkFactory::create<logSys::RollBySizeSink>("./logdir/mylog", 1024 * 1024);
    int cursize = 0;
    int count = 0;
    while(cursize < 1024 * 1024 * 10)
    {
        std::string msg = ret + std::to_string(count++);
        psink->log(msg.c_str(), msg.size());
        cursize += msg.size();
    }
*/
    // std::cout << mylog::util::Date::now() << std::endl;
    // mylog::util::File::createDirectory(mylog::util::File::path("abc/def/a.txt"));
    
    // std::cout << logSys::LogLevel::toString(logSys::LogLevel::Level::DEBUG) << std::endl;
    // std::cout << logSys::LogLevel::toString(logSys::LogLevel::Level::INFO) << std::endl;
    // std::cout << logSys::LogLevel::toString(logSys::LogLevel::Level::WARNING) << std::endl;
    // std::cout << logSys::LogLevel::toString(logSys::LogLevel::Level::ERROR) << std::endl;
    // std::cout << logSys::LogLevel::toString(logSys::LogLevel::Level::FATAL) << std::endl;
    return 0;
}