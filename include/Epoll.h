#pragma once

#include <vector>

class Channel;

class Epoll 
{
public:
    Epoll();
    ~Epoll();

public:
    void updateChannel(Channel* ch);
    //等待事件发生，timeout 毫秒，默认 -1 (一直等)
    std::vector<Channel*> poll(int timeout = -1);

private:
    int _epoll_fd;
    struct epoll_event * _events;
};