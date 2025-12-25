#include "../include/Buffer.h"
#include <unistd.h>
#include <sys/uio.h>
#include <algorithm>

/*
[ Prepend可加区 ] [ Readable可读区 (已有数据) ] [ Writable可写区 (空白) ]
^                ^                            ^
|                |                            |
data()        readIndex                   writeIndex
*/

Buffer::Buffer(size_t initialSize) : _buffer(initialSize) {}

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
//外部接口
void Buffer::append(const char* data, size_t len) {
    //可写空间不够 扩容
    if (writableBytes() < len) {
        makeSpace(len);
    }
    std::copy(data, data + len, _buffer.begin() + _writeIndex);
    _writeIndex += len;
}
void Buffer::append(std::string_view data) {
    append(data.data(), data.size());
}
//内部接口
void Buffer::makeSpace(size_t len) {
    if(writableBytes() + prependableBytes() < len) {
        // 情况1：总空间真的不够，必须 resize
        _buffer.resize(_writeIndex + len);
    }
    else {
        // 情况2：总空间够，只需把存量数据“挪”到开头
        ssize_t readable = readableBytes();
        std::copy(_buffer.begin() + _readIndex, 
                    _buffer.begin() + _writeIndex,
                    _buffer.begin());
        _readIndex = 0;
        _writeIndex = readable;
    }
}

ssize_t Buffer::readFd(int fd, int* saveErrno) {
    char extrabuf[65536];//64kb 的栈空间 
    struct iovec vec[2];
    const size_t writable = writableBytes();

    // 第一块：Buffer 里的剩余空间
    vec[0].iov_base = _buffer.data() + _writeIndex;//剩余空间首地址
    vec[0].iov_len = writable;//剩余空间长度
    // 第二块：栈上的 extrabuf
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    // 如果 Buffer 够大，只用第一块；如果 Buffer 比较小，就配上备用口袋
    const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
    //用readv 读取 fd里面的数据 根据大小 选择放入第一个buffer中 或是第一个＋第二个  
    const ssize_t n = ::readv(fd, vec, iovcnt); //这里readv会将数据写入buf 或 extrabuf 里面 然后返回一个n

    if(n < 0) {
        *saveErrno = errno;
    } 
    else if (static_cast<size_t>(n) <= writable) {
        // 情况 A: 全部填入 Buffer 内部空间里
        _writeIndex += n;
    }
    else {
        // 情况 B:  Buffer 填满了 接下来的数据需要放入 extrabuf  ----此注释会有歧义
        // 上面已经写入到extrabuf里面了 （这里是通过判断n的大小 知道还有部分数据保存在extra里面的）
        // 这里实际是 通过append 将extrabuf里面的数据写入到buffer里面 ----buffer扩容实现的 
        _writeIndex = _buffer.size();
        append(extrabuf, static_cast<size_t>(n) - writable);
    }
    return n;
}

std::string Buffer::retrieveAsString(size_t len) {
    std::string result(peekAsView().data(), len);
    retrieve(len);
    return result;
}   

std::string Buffer::retrieveAllAsString() {
    return retrieveAsString(readableBytes());
}

const char* Buffer::findCRLF() {
    static const char CRLF[] = "\r\n";
    const char* start = _buffer.data() + _readIndex;
    const char* end = _buffer.data() + _writeIndex;
    const char* crlf = std::search(start, end, CRLF, CRLF + 2);
    return crlf == end ? nullptr : crlf;
}