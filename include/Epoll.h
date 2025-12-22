#pragma once

#include <vector>
#include <sys/epoll.h>

class Channel;

class Epoll 
{
public:
    Epoll();
    ~Epoll();
    //禁止拷贝
    Epoll(const Epoll&) = delete;
    Epoll& operator=(const Epoll&) = delete;

public:
    // --- 核心接口 ---
    void updateChannel(Channel* ch);
    void removeChannel(Channel* ch);
    
    // 巡检接口 ：返回发生了事件的列表
    [[nodiscard]] std::vector<Channel*> poll(int timeout = -1);

private:
    int _epoll_fd{-1};
    std::vector<struct epoll_event> _events;
};