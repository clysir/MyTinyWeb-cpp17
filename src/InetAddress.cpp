#include "../include/InetAddress.h"
#include <arpa/inet.h>
#include <string>
#include <cstring>

InetAddress::InetAddress(const std::string &ip,  uint16_t port) : _ip(ip), _port(port) {
    memset(&_addr,0,sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(_ip.c_str());
    _addr.sin_port = htons(_port);
}