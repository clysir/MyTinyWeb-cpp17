#pragma once

#include <unistd.h>

class InetAddress;

class Socket 
{
public:
    Socket();
    // 显式构造，用于 Accept 拿到的 fd
    explicit Socket(int fd) noexcept : _fd(fd) {} 
    // 禁止拷贝，只能移动（因为 fd 是独占资源）
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    ~Socket();

public:
    [[nodiscard]] int getFd() const noexcept { return _fd; }
    void bindAddress(const InetAddress& addr);
    void listen();
    [[nodiscard]] int accept(InetAddress& peeraddr);

    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setNonBlocking();

private:
    int _fd{-1};
};