#include "../include/Channel.h"
#include "../include/Epoll.h"
#include <sys/epoll.h>

Channel::Channel(Epoll * ep,int fd) : _ep(ep),_fd(fd) {};

void Channel::enableReading() {
    _events = EPOLLIN | EPOLLET;
    _ep->updateChannel(this);
}
//回调函数 判断读 还是 写
void Channel::handleEvent() {
    if(_revents & EPOLLIN) { 
        if(_readCallback) _readCallback();
    }
    if(_revents & EPOLLOUT) { 
        if(_writeCallback) _writeCallback();
    }
}

Channel::~Channel() {

}