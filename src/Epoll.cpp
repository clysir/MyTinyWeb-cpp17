#include "../include/Epoll.h"
#include "../include/Channel.h"
#include <unistd.h>
#include <iostream>

// 初始监听事件数量
static constexpr int kInitEventListSize = 16;

Epoll::Epoll() 
    : _epoll_fd{::epoll_create1(EPOLL_CLOEXEC)},
      _events(kInitEventListSize)
{
    if (_epoll_fd < 0) {
        std::cerr << "Epoll create failed, error: " << errno << std::endl;
    }
}

void Epoll::updateChannel(Channel* ch) {
    struct epoll_event ev{};//将内存清零
    ev.events = ch->getEvents();
    ev.data.ptr = ch;//将指针存入

    int fd = ch->getFd();
    const int op = ch->isInEpoll() ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;

    if(::epoll_ctl(_epoll_fd, op, fd, &ev) < 0) {
        std::cerr << "Epoll update failed, op: " << op << ", fd: " << fd << std::endl;
    } else {
        ch->setInEpoll(true);
    }
}

void Epoll::removeChannel(Channel* ch) {
    if(ch->isInEpoll()) {
        ::epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, ch->getFd(), nullptr);
        ch->setInEpoll(false);
    }
}

std::vector<Channel*> Epoll::poll(int timeout) {
    // 调用内核接口
    int numEvents = ::epoll_wait(_epoll_fd, _events.data(), static_cast<int>(_events.size()), timeout);

    std::vector<Channel*> activeChannels;
    if(numEvents > 0) {
        for(int i = 0; i < numEvents; i++) {
            Channel* ch = static_cast<Channel*>(_events[i].data.ptr);
            ch->setRevents(_events[i].events);
            activeChannels.push_back(ch);
        }

        //如果事件满了 就下次扩容
        if(static_cast<size_t>(numEvents) == _events.size()) {
            _events.resize(_events.size() * 2);
        }
    } else if(numEvents < 0) {
        if(errno != EINTR) { // 排除被信号中断的情况
            std::cerr << "epoll_wait errno, errno: " << errno << std::endl;
        }
    }

    return activeChannels;
}

Epoll::~Epoll() {
    if (_epoll_fd != -1) {
        ::close(_epoll_fd);
    }
}