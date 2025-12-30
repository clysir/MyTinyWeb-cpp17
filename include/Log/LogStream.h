#pragma once

#include <sstream>
#include <string>
#include <string_view>
#include "LogLevel.h"

class LogStream 
{
public:
    LogStream& operator<<(int val);
    LogStream& operator<<(const char* str);
    LogStream& operator<<(const std::string& str);
    LogStream& operator<<(const LogLevel& level);
    LogStream& operator<<(bool val);
    LogStream& operator<<(size_t val);
    LogStream& operator<<(std::string_view sv);
    
    std::string str() const;

private:
    std::ostringstream _buffer;
};