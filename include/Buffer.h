#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <sys/types.h>

class Buffer 
{
public:
    explicit Buffer(size_t initialSize = 1024); 
    ~Buffer() = default;

public:
    //基础信息
    [[nodiscard]] size_t readableBytes() const noexcept { return _writeIndex - _readIndex; }
    [[nodiscard]] size_t writableBytes() const noexcept { return _buffer.size() - _writeIndex; }
    //可加空间
    [[nodiscard]] size_t prependableBytes() const noexcept { return _readIndex; }

    //读写接口
    void append(const char* data, size_t len);
    void append(std::string_view data);
    //return {A, B} 其实是返回了一个string_view"对象"
    [[nodiscard]] std::string_view peekAsView() const noexcept { return {&_buffer[_readIndex], readableBytes()}; }
    void retrieve(size_t len);
    void retrieveAll();

    //把缓冲区数据 转换成string 
    [[nodiscard]] std::string retrieveAsString(size_t len);
    [[nodiscard]] std::string retrieveAllAsString();

    //核心IO接口
    ssize_t readFd(int fd, int* saveErrno);

    [[nodiscard]] const char* findCRLF();

private:
    void makeSpace(size_t len);//内部移动数据 或 扩容逻辑
private:
    std::vector<char> _buffer;
    size_t _readIndex{0};//指向可读数据的起始位置
    size_t _writeIndex{0};//指向可写数据的起始位置
};