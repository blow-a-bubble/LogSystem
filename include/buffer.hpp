#pragma once
#include <vector>
#include <string>
#include <cassert>
/*
    自定义缓冲区
*/
namespace logSys
{
    #define BUFFER_DEFAULT_SIZE (1*1024*1024) 
    #define BUFFER_INCREMENT_SIZE (1*1024*1024) // 线性增长值
    #define BUFFER_THRESHOLD_SIZE (10*1024*1024) // 缓冲区增容界限，在这之前两倍增长，之后线性增长
    class Buffer
    {
    public: 
        Buffer():_buffer(BUFFER_DEFAULT_SIZE), _write_idx(0), _read_idx(0)
        {}
        char *begin()
        {
            return &_buffer[0];
        }
        bool empty()
        {
            return _write_idx == _read_idx;
        }
        // 移动读指针
        void moveReadBack(size_t len)
        { 
            assert(len <= readAbleSize());
            _read_idx += len;
        }
        // 移动写指针
        void moveWriteBack(size_t len)
        {
            assert(len <= tailIdleSize());
            _write_idx += len;
        }
        // 获取读位置
        char *readPositon()
        {
            return &_buffer[_read_idx];
        }
        // 获取写位置
        char *writePosition()
        {
            return &_buffer[_write_idx];
        }
        // 头部空闲大小
        size_t headIdleSize()
        {
            return _read_idx;
        }
        // 尾部空闲大小
        size_t tailIdleSize()
        {
            return _buffer.size() - _write_idx;
        }
        // 可读大小
        size_t readAbleSize()
        {
            return _write_idx - _read_idx;
        }
        // 可写大小
        size_t writeAbleSize()
        {
            return headIdleSize() + tailIdleSize();
        }
        // 写数据并且移动指针
        void writeAndPush(const char *data, size_t len)
        {   
            write(data, len);
            moveWriteBack(len);
        }
        // 写数据
        void write(const char *data, size_t len)
        {
            ensureWriteAble(len);
            std::copy(data, data + len, writePosition());
        }
        // 读数据并且移动指针
        void readAndPop(char *data, size_t len)
        {
            read(data, len);
            moveReadBack(len);
        }
        // 读数据
        void read(char *data, size_t len)
        {
            assert(len <= readAbleSize());
            std::copy(readPositon(), readPositon() + len, data);
        }
        // 保证能写
        void ensureWriteAble(size_t len)
        {
            // 尾部有足够空间
            if(len <= tailIdleSize()) return;
            // 缓冲区大小足够
            if(len <= tailIdleSize() + headIdleSize())
            {
                std::copy(readPositon(), writePosition(), begin());
                _read_idx = 0, _write_idx = len;
            }
            // 空间不够增容
            else
            {
                if(_buffer.size() < BUFFER_THRESHOLD_SIZE)
                {
                    _buffer.resize(_buffer.size() * 2 + len);
                }
                else
                {
                    _buffer.resize(_buffer.size() + BUFFER_INCREMENT_SIZE + len);
                }
            }
        }
        // 交换缓冲区
        void swap(Buffer &buffer)
        {
            _buffer.swap(buffer._buffer);
            std::swap(_read_idx, buffer._read_idx);
            std::swap(_write_idx, buffer._write_idx);
        }
        // 重置缓冲区
        void reset()
        {
            _read_idx = _write_idx = 0;
        }
    private:
        std::vector<char> _buffer;
        size_t _read_idx;
        size_t _write_idx;
    };
}