#include "../../include/Log/LogStream.h"
#include "../../include/Log/LogLevel.h"
	
LogStream& LogStream::operator<<(int val) {
    _buffer << val;
    return *this;
}

LogStream& LogStream::operator<<(const char* str) {
    _buffer << str;
    return *this;
}

LogStream& LogStream::operator<<(const std::string& str) {
    _buffer << str;
    return *this;
}

LogStream& LogStream::operator<<(const LogLevel& level) {
    _buffer << levelToString(level);
    return *this;
}

LogStream& LogStream::operator<<(bool val) {
    _buffer << (val ? "true" : "false");
    return *this;
}

LogStream& LogStream::operator<<(size_t val) {
    _buffer << val;
    return *this;
}

LogStream& LogStream::operator<<(std::string_view sv) {
    _buffer << sv;
    return *this;
}

std::string LogStream::str() const {
    return _buffer.str();
}