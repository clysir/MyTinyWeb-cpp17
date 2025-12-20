#include "../include/Socket.h"
#include "../include/InetAddress.h"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

Socket::Socket() {
    _fd = socket(AF_INET,SOCK_STREAM,0);
    if (_fd < 0) {
        perror("socket failed");
        exit(-1);
    }
}

void Socket::Bind(const InetAddress& addr) {
   if( bind(_fd,addr.GetSockAddr(),addr.GetSockAddrLen()) < 0 ) {
       perror("Bind failed");
       exit(-1);
   }

}

void Socket::Listen() {
    if (listen(_fd,5) < 0) {
        perror("Listen failed");
        exit(-1);
    }
}

void Socket::SetNonBlocking() {
    fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
}

int Socket::Accept(InetAddress& addr) {
    socklen_t len = sizeof(struct sockaddr_in);
    int accept_fd = accept(_fd,addr.GetSockAddr(),&len);
    if (accept_fd < 0) {
        perror("Accept failed");
        exit(-1);
    }
    return accept_fd;
}