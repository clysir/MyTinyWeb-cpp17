#include "../include/Buffer.h"

Buffer::Buffer() {
    _readIndex = 0;
    _writeIndex = 0;
    _buffer.resize(1024);
}

size_t Buffer::readableBytes() const {
    return _writeIndex - _readIndex;
}

size_t Buffer::writableBytes() const {
    return _buffer.size() - _writeIndex;
}

const char* Buffer::peek() const {
    return &_buffer[_readIndex];
}

void Buffer::retrieve(size_t len) {
    if (len < readableBytes()) {
        _readIndex += len;
    } else {
        retrieveAll();
    }
}

void Buffer::retrieveAll() {
    _readIndex = 0;
    _writeIndex = 0;
}

void Buffer::append(const char* data, size_t len) {
    //可写空间不够 扩容
    if (writableBytes() < len) {
        //总剩余空间 = 剩余可写空间 + 前面已读空间
        if ((writableBytes() + _readIndex) < len) {
            // 情况2：总空间真的不够，必须 resize
            _buffer.resize(_writeIndex + len);
        }
        else {
            // 情况1：总空间够，只需把存量数据“挪”到开头
            ssize_t readable = readableBytes();
            std::copy(_buffer.begin() + _readIndex, 
                        _buffer.begin() + _writeIndex,
                        _buffer.begin());
            _readIndex = 0;
            _writeIndex = readable;
        }
    }
    std::copy(data, data + len, _buffer.begin() + _writeIndex);
    _writeIndex += len;
}

ssize_t Buffer::readFd(int fd, int* saveErrno) {
    //暂时先不去实现
}

Buffer::~Buffer() {
    _buffer.clear();
}