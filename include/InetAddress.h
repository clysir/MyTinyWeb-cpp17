#pragma once

#include <arpa/inet.h>
#include <string>
#include <string_view>
#include <cstdint>

class InetAddress 
{
public:
    InetAddress() = default;
    InetAddress(std::string_view ip, uint16_t port);  
    explicit InetAddress(const sockaddr_in& addr) : _addr{addr} {}
    ~InetAddress() {}

public:
    //reinterpret_cast<>是cpp里的强制类型转换
    [[nodiscard]] struct sockaddr* getSockAddr() noexcept { return reinterpret_cast<struct sockaddr*>(&_addr); } 
    [[nodiscard]] const struct sockaddr* getSockAddr() const noexcept { return reinterpret_cast<const struct sockaddr*>(&_addr); } 
    [[nodiscard]] socklen_t getSockAddrLen() const noexcept { return sizeof(_addr); }
     
    [[nodiscard]] std::string toIp() const;
    [[nodiscard]] uint16_t toPort() const;
private:
    struct sockaddr_in _addr{};//默认初始化为0
    uint16_t _port;
};