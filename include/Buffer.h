#pragma once

#include <vector>

class Buffer 
{
public:
    Buffer(size_t initialSize = 1024); 
    ~Buffer();

public:
    //基础信息
    size_t readableBytes() const;
    size_t writableBytes() const;
    //读写接口
    void append(const char* data, size_t len);
    const char* peek() const;
    void retrieve(size_t len);
    void retrieveAll();
    //核心IO接口
    ssize_t readFd(int fd, int* saveErrno);

private:
    std::vector<char> _buffer;
    size_t _readIndex;//指向可读数据的起始位置
    size_t _writeIndex;//指向可写数据的起始位置
};