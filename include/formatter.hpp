#pragma once
#include "util.hpp"
#include "message.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <ctime>
#include <cassert>
namespace logSys
{
    /*
        %d 日期
        %T 缩进
        %t 线程id
        %p 日志级别
        %c 日志器名称
        %f 文件名
        %l 行号
        %m 日志消息
        %n 换行
    */
    // 抽象格式化子项
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual ~FormatItem() = default;
        virtual void format(std::ostream &os, const LogMsg &msg) = 0;
    };
    class TimeFormatItem : public FormatItem
    {
    public:
        TimeFormatItem(const std::string &format = "%H%M%S")
        :_format(format)
        {}
        void format(std::ostream &os, const LogMsg &msg) override
        {
            struct tm t;
            localtime_r(&msg._ctime, &t);
            char tm[128] = { 0 };
            strftime(tm, 127, _format.c_str(), &t);
            os << tm;
        }
    private:
        std::string _format;
    };

    class TabFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &os, const LogMsg &msg) override
        {
            os << "\t";
        }
    };

    class ThreadFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &os, const LogMsg &msg) override
        {
            os << msg._pid;
        }
    };

    class LevelFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &os, const LogMsg &msg) override
        {
            os << LogLevel::toString(msg._level);
        }
    };

    class NameFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &os, const LogMsg &msg) override
        {
            os << msg._name;
        }
    };

    class FileFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &os, const LogMsg &msg) override
        {
            os << msg._file;
        }
    };

    class LineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &os, const LogMsg &msg) override
        {
            os << msg._line;
        }
    };

    class MsgFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &os, const LogMsg &msg) override
        {
            os << msg._payload;
        }
    };

    class NLineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &os, const LogMsg &msg) override
        {
            os << "\n";
        }
    };

    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string &format)
        :_format(format)
        {}
        void format(std::ostream &os, const LogMsg &msg) override
        {
            os << _format;
        }
    private:
        std::string _format;
    };
    
    class Formatter
    {
    public:
        using ptr = std::shared_ptr<Formatter>;
        //  默认格式 "%d{%H:%M:%S}%T%t%T[%p]%T[%c]%T%f:%l%T%m%n"
        Formatter(const std::string &pattern = "%d{%H:%M:%S}%T%t%T[%p]%T[%c]%T%f:%l%T%m%n")
        :_pattern(pattern)
        {
            assert(parsePattern());
        }
        // 将日志消息格式化
        void format(std::ostream &os, const LogMsg &msg)
        {
            for(auto & it : _items)
            {
                it->format(os, msg);
            }
        }
        // 将日志消息格式化
        std::string format(const LogMsg &msg)
        {
            std::stringstream ss;
            format(ss, msg);
            return ss.str();
        }
    private:
        bool parsePattern()
        {
            // sg{}fsg%d{%H:%M:%S}%Tsdf%t%T[%p]%T[%c]%T%f:%l%T%m%n 
            int index = 0;
            while(index < _pattern.size())
            {
                std::string key, value;
                // 1.提取原始字符串
                auto pos = _pattern.find("%", index);
                if(pos == std::string::npos)
                {
                    value = _pattern.substr(index);
                    _items.push_back(createItem(key, value));
                    break;
                }
                else if(pos == index)
                {
                    // 没有原始字符串
                }
                else
                {
                    value = _pattern.substr(index, pos - index);
                    _items.push_back(createItem(key, value));
                }

                // 走到%的下一个字符
                index = pos + 1;
                if(index >= _pattern.size())
                {
                    std::cout << "解析失败，只有%没有解析字符\n";
                    return false;
                }
                // 如果是%%代表原始字符串%
                if(_pattern[index] == '%')
                {
                    _items.push_back(createItem("", "%"));
                    index++;
                    continue;
                }
                // 2.提取格式化子项
                else
                {
                    key = _pattern[index];
                    // 看看是否有value, 比如 %d{%H:%M:%S}
                    if(index + 1 < _pattern.size() && _pattern[index + 1] == '{')
                    {
                        index += 2; // 走到非{字符
                        pos = _pattern.find('}', index);
                        if(pos == std::string::npos)
                        {
                            std::cout << "没有找到 } ,解析失败\n";
                            return false;
                        }
                        else 
                        {
                            value = _pattern.substr(index, pos - index);
                            _items.push_back(createItem(key, value));
                            index = pos + 1;
                        }
                    }
                    else
                    {
                        _items.push_back(createItem(key, value));
                        index++;
                    }
                }
            }
            return true;
        }
        FormatItem::ptr createItem(const std::string &key, const std::string &value)
        {
            // key为空表示原始字符串, 原始字符串在value
            if(key.empty())
            {
                return std::make_shared<OtherFormatItem>(value);
            }
            // %d{%H:%M:%S}%T%t%T[%p]%T[%c]%T%f:%l%T%m%n
            // 正常格式化字符
            if(key == "d") return std::make_shared<TimeFormatItem>(value);
            else if(key == "T") return std::make_shared<TabFormatItem>();
            else if(key == "t") return std::make_shared<ThreadFormatItem>();
            else if(key == "p") return std::make_shared<LevelFormatItem>();
            else if(key == "c") return std::make_shared<NameFormatItem>();
            else if(key == "f") return std::make_shared<FileFormatItem>();
            else if(key == "l") return std::make_shared<LineFormatItem>();
            else if(key == "m") return std::make_shared<MsgFormatItem>();
            else if(key == "n") return std::make_shared<NLineFormatItem>();
            else
            {
                std::cout << "没有找到合适的格式化符: " << key << "\n";
                abort();
            }
        }
    private:
        std::string _pattern; // 格式化模式
        std::vector<FormatItem::ptr> _items; // 格式化子项
    };
}