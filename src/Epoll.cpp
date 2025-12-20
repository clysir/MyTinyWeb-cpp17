#include "../include/Epoll.h"
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <Channel.h>
#include <sys/epoll.h>

Epoll::Epoll() {
    _epoll_fd = epoll_create1(0);
    // 注意这不是支持的最大连接数，而是一次 epoll_wait 能返回的最大事件数
    _events = new struct epoll_event[1024];
}

void Epoll::updateChannel(Channel* ch) {
    int fd = ch->getFd();
    struct epoll_event ev;
    ev.events = ch->getEvents();
    ev.data.ptr = ch;//将指针存入

    if(ch->isInEpoll()) {
        //已经在树上，此时修改
        epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev);
    } else {
        //不在树上，此时添加
        epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
        ch->setInEpoll(true);
    }
}

std::vector<Channel*> Epoll::poll(int timeout) {
    std::vector<Channel*> channels;
    int nfds = epoll_wait(_epoll_fd, _events, 1024, timeout);
    for(int i = 0; i < nfds; i++) {
        Channel* ch = (Channel*)_events[i].data.ptr;
        ch->setRevents(_events[i].events);//告诉Channel 发生了什么事
        channels.push_back(ch);
    }
    return channels;
}

Epoll::~Epoll() {
    if (_epoll_fd != -1) {
        close(_epoll_fd);
        _epoll_fd = -1;
    }
    delete[] _events;
}