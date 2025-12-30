#include "../../include/Log/Logger.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

std::function<void(const std::string&)> Logger::_outputFunc = 
                                        [] (const std::string& log) { std::cout << log; };

// 获取当前时间的辅助函数
static std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* timeinfo = std::localtime(&now_c);
    std::ostringstream oss;
    oss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

Logger::Logger(LogLevel level, const char* file, int line) 
    : _level(level), _file(file), _line(line) {}

Logger::~Logger() {
    // 获取时间戳
    std::string timestamp = getCurrentTime();

    std::string levelStr = levelToString(_level);

    // 组成完整日志
    std::string log = timestamp + " [" + levelStr + "] "
                    + _file + ":" + std::to_string(_line) + "-"
                    + _stream.str() + "\n";

    // 输出日志
    if(_outputFunc) {
        _outputFunc(log);
    }
}