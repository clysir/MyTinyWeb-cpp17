#pragma once 

#include <string>

enum class LogLevel 
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL   
};

// inline 是可以防止多个.cpp 文件添加此头文件时 避免重复定义
inline const char* levelToString(LogLevel level) {
    switch(level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}
