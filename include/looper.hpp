#pragma once
#include "buffer.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <atomic>
namespace logSys
{
    // 异步缓冲区是否安全: 安全即缓冲区定长，不安全相反
    enum class AsyncType
    {
        AsyncSafe,
        AsyncUnSafe
    };
    class AsyncLooper 
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;
        using Functor = std::function<void(Buffer &)>;
        AsyncLooper(const Functor& callback, AsyncType is_safe)
        :_is_safe(is_safe), _callback(callback), _running(true),
        _thread(&AsyncLooper::threadEntry, this)
        {}
        ~AsyncLooper()
        {
            _running = false;
            _cond_consumer.notify_all();
            _thread.join();
            
        }
        void push(const char* data, size_t len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if(_is_safe == AsyncType::AsyncSafe)
            {
                _cond_producer.wait(lock, [&](){ return len <= _buffer_producer.writeAbleSize(); });
            }
            _buffer_producer.writeAndPush(data, len);
            _cond_consumer.notify_one();
        }
        void push(const std::string &data)
        {
            push(data.c_str(), data.size());
        }
    private:
        void threadEntry()
        {
            // 走到这里代表第一次进入和消费完_buffer_consumer，缓冲区都是没有数据的
            while(1)
            {
                // lock的声明周期随 {}
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    if(!_running && _buffer_producer.empty()) return;
                    _cond_consumer.wait(lock, [&](){
                        return !_running || !_buffer_producer.empty();
                    });
                    _buffer_consumer.swap(_buffer_producer);
                }
                // 交换缓冲区后唤醒生产者线程
                _cond_producer.notify_all();
                _callback(_buffer_consumer);
                _buffer_consumer.reset();
            }
        }
    private:
        // 双缓冲区机制减少锁竞争
        Buffer _buffer_producer; // 生产者缓冲区
        Buffer _buffer_consumer; // 消费者缓冲区
        std::thread _thread; // 异步工作线程
        std::atomic<bool> _running; // 是否工作
        Functor _callback; // 日志落地回调
        std::mutex _mutex; 
        std::condition_variable _cond_producer;
        std::condition_variable _cond_consumer;
        AsyncType _is_safe;
    };
}