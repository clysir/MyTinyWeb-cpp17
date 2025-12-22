#pragma once

#include "Epoll.h"
#include <memory>

class Channel;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop() = default;
    //禁止拷贝
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

public:
    // 核心接口: 开始无限循环
    void loop();
    //优雅退出
    void quit();
    // 给 Channel 用: 把自己(Channel)更新到 Epoll 树上
    void updateChannel(Channel* ch);
    void removeChannel(Channel* ch);
    [[nodiscard]] Epoll* getEpoll() const noexcept { return _ep.get(); }

private:
    std::unique_ptr<Epoll> _ep;
    bool _quit{false}; // 标记是否退出
};
