#include "logSys.h"
#include <vector>
#include <thread>
#include <chrono>
namespace logSys
{
    // 性能测试
    void bench(const std::string &name, size_t thread_num, size_t msg_len, size_t msg_num)
    {
        using namespace std::chrono;
        // 1.获取日志器
        Logger::ptr lp = getLogger(name);
        if(lp.get() == nullptr) return;

        // 2.根据参数创建日志消息
        std::string msg(msg_len - 1, 'a'); // 预留一个位置给\n
        size_t thread_msg_len = msg_num / thread_num;

        // 3.创建线程
        std::vector<std::thread> threads;
        std::vector<size_t> times(thread_num);
        for (size_t i = 0; i < thread_num; i++)
        {
            threads.emplace_back([&, i](){
                // 4.输出日志并统计时间
                auto start = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < thread_msg_len; i++)
                {
                    lp->fatal("%s", msg.c_str());
                }
                auto end = high_resolution_clock::now();
                duration<double> time_span = duration_cast<duration<double>>(end - start);
                times[i] = time_span.count();
                std::cout << "线程: " << i << " 输出日志数量: " << thread_msg_len 
                << " 耗时: " << times[i] << "s"<< std::endl;
            });
        }
        // 5.等待并回收线程
        for (size_t i = 0; i < thread_num; i++)
            threads[i].join();
        // 6.计算总时间
        size_t max_time = times[0];
        for (size_t i = i; i < thread_num; i++)
            max_time = times[i] > max_time ? times[i] : max_time;
        // 7.打印性能结果
        std::cout << "总消耗时间: " << max_time << std::endl;
        if(max_time == 0)
        {
            std::cout << "太快了，请增加测试用例\n";
            return;
        } 
        std::cout << "平均每秒输出: " << (size_t)(msg_num / max_time) << "条日志" << std::endl;
        std::cout << "平均每秒输出: " << (size_t)(msg_len * msg_num / max_time / 1024 / 1024) << "MB" << std::endl;
    }
    void bench_sync(size_t thread_num, size_t msg_len, size_t msg_num)
    {
        LoggerBuilder::ptr builder = std::make_shared<GlobalLoggerBuilder>();
        builder->buildFormatter("%m%n");
        builder->buildLimitLevel(LogLevel::Level::DEBUG);
        builder->buildLoggerName("sync_logger");
        builder->buildLoggerType(logSys::LoggerType::LOGGER_SYNC);
        builder->buildSink<FileSink>("./logdir/sync_logger.log");
        Logger::ptr lp = builder->build();
        bench(lp->getName(), thread_num, msg_len, msg_num);
    }
    void bench_async(size_t thread_num, size_t msg_len, size_t msg_num)
    {
        LoggerBuilder::ptr builder = std::make_shared<GlobalLoggerBuilder>();
        builder->buildFormatter("%m%n");
        builder->buildLimitLevel(LogLevel::Level::DEBUG);
        builder->buildLoggerName("async_logger");
        builder->buildLoggerType(logSys::LoggerType::LOGGER_SYNC);
        builder->buildSink<FileSink>("./logdir/async_logger.log");
        builder->buildAsyncType(AsyncType::AsyncUnSafe); // 仅性能测试使用
        Logger::ptr lp = builder->build();
        bench(lp->getName(), thread_num, msg_len, msg_num);
    }
}

int main()
{
    logSys::bench_async(4, 100, 2e7);
    return 0;
}