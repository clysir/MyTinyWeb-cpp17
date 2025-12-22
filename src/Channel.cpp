#include "../include/Channel.h"
#include "../include/Epoll.h"
#include <sys/epoll.h>

Channel::Channel(Epoll * ep,int fd) : _ep(ep), _fd(fd) {}

// 拨动开关后，必须点一下“同步”
void Channel::update() {
    _ep->updateChannel(this);
}

void Channel::enableReading() {
    _events = EPOLLIN | EPOLLPRI;
    update();
}

void Channel::enableWriting() {
    _events |= EPOLLOUT ;
    update();
}

void Channel::disableReading() {
    _events &= ~EPOLLIN;
    update();
}

void Channel::disableWriting() {
    _events &= ~EPOLLOUT;
    update();
}
 
void Channel::disableAll() {
    _events = 0;
    update();
}

//核心分发逻辑
void Channel::handleEvent() {
    // 1. 处理关闭：(挂断且当前没有可读数据)
    if((_revents & EPOLLHUP) && !(_revents & EPOLLIN)) {
        if(_closeCallback) _closeCallback();
    }

    // 2. 处理错误
    if(_revents & EPOLLERR) {
        if(_errorCallback) _errorCallback();
    }

    // 3. 处理读事件(IN:可读 PRI：紧急 RDHUP：对端关闭)
    if(_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if(_readCallback) _readCallback();
    }

    // 4. 处理写事件
    if(_revents & EPOLLOUT) {
        if(_writeCallback) _writeCallback();
    }
}

Channel::~Channel() {}