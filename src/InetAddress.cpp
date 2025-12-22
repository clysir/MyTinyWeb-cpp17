#include "../include/InetAddress.h"
#include <arpa/inet.h>
#include <cstring>

InetAddress::InetAddress(std::string_view ip,  uint16_t port) {
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(std::string(ip).c_str());
    _addr.sin_port = htons(port);
}

std::string InetAddress::toIp() const {
    char buf[64] = {0};
    // ::inet_ntop 是更安全、现代的函数，它支持 IPv4 和 IPv6
    ::inet_ntop(AF_INET, &_addr.sin_addr, buf, sizeof(buf));
    return buf;
}

uint16_t InetAddress::toPort() const {
    // 从网络字节序转回主机字节序
    return ntohs(_addr.sin_port);
}