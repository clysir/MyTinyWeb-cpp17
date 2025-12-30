#pragma once

#include "LogLevel.h"
#include "LogStream.h"
#include <functional>


class Logger
{
public:
    Logger(LogLevel level, const char* file, int line); 
    ~Logger(); // 析构时 输出完整日志

    LogStream& stream() { return _stream; }

    static void setOutputFunc(std::function<void(const std::string&)> func) { _outputFunc = std::move(func); }

private:
    LogLevel _level;
    LogStream _stream;
    const char* _file;
    int _line;
    static std::function<void(const std::string&)> _outputFunc;// 静态变量 用于控制 输出到哪里 默认是cout
};

#define LOG_DEBUG Logger(LogLevel::DEBUG, __FILE__, __LINE__).stream()
#define LOG_INFO  Logger(LogLevel::INFO, __FILE__, __LINE__).stream()
#define LOG_WARN  Logger(LogLevel::WARN, __FILE__, __LINE__).stream()
#define LOG_ERROR Logger(LogLevel::ERROR, __FILE__, __LINE__).stream()
#define LOG_FATAL Logger(LogLevel::FATAL, __FILE__, __LINE__).stream()