#pragma once

class Epoll;
class Channel;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

public:
    // 核心接口: 开始无限循环
    void loop();
    // 给 Channel 用: 把自己(Channel)更新到 Epoll 树上
    void updateChannel(Channel* ch);
    Epoll* getEpoll() { return _ep; }
private:
    Epoll* _ep;
    bool _quit; // 标记是否退出
};
