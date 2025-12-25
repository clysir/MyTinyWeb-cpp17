#include "../include/Socket.h"
#include "../include/InetAddress.h"
#include <sys/socket.h>
#include <iostream>
#include <fcntl.h>

Socket::Socket() {
    _fd = ::socket(AF_INET,SOCK_STREAM,0);
    if (_fd < 0) {
        std::cerr << "Socket failed, error: " << errno << std::endl;
    }
    setReuseAddr(true);
    setReusePort(true);
}
//设置地址复用
void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    // SOL_SOCKET 代表设置 Socket 层面的属性
    ::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::bindAddress(const InetAddress& addr) {
   if( ::bind(_fd, addr.getSockAddr(), addr.getSockAddrLen()) < 0 ) {
       std::cerr << "Bind failed, error: " << errno << std::endl;
   }

}

void Socket::listen() {
    if ( ::listen(_fd,5) < 0) {
        std::cerr << "Listen failed, error: " << errno << std::endl;
    }
}

void Socket::setNonBlocking() {
    fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress& peeraddr) {
    socklen_t len = sizeof(struct sockaddr_in);
    // 使用 accept4 原子地设置非阻塞和 exec 关闭标志
    int accept_fd = ::accept4(_fd, peeraddr.getSockAddr(), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (accept_fd < 0) {
        std::cerr << "Accept failed, error: " << errno << std::endl;
    }
    return accept_fd;
}

Socket::~Socket() {
    if (_fd != -1) {
        ::close(_fd);
    }
}
