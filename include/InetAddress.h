#pragma once

#include <arpa/inet.h>
#include <string>
#include <cstdint>

class InetAddress 
{
public:
    InetAddress() = default;
    InetAddress(const std::string& ip, uint16_t port);  
    ~InetAddress() {}

public:
    struct sockaddr* GetSockAddr() { return (struct sockaddr*)&_addr; } 
    const struct sockaddr* GetSockAddr() const { return (const struct sockaddr*)&_addr; } 
    socklen_t GetSockAddrLen() { return _addr_len; }
    socklen_t GetSockAddrLen() const { return _addr_len; }
    
private:
    struct sockaddr_in _addr;
    socklen_t _addr_len = sizeof(_addr);
    std::string _ip;
    uint16_t _port;
};