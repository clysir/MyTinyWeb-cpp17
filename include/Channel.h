#pragma once

#include <functional>
#include <cstdint>
#include <sys/epoll.h>

class Epoll;

class Channel
{
public:
    // 定义回调函数的类型：void() 不需要参数
    using Callback = std::function<void()>;

    Channel(Epoll* ep,int fd);
    ~Channel();

public:
    // 核心：由 EventLoop 调用，分发不同类型的事件
    void handleEvent(); 

    // --- 事件开关接口  ---
    void enableReading();
    void enableWriting();
    void disableReading();
    void disableWriting();
    void disableAll();

    // --- 状态查询-----
    void setReadCallback(Callback cb) { _readCallback = std::move(cb); }
    void setWriteCallback(Callback cb) { _writeCallback = std::move(cb); }
    void setErrorCallback(Callback cb) { _errorCallback = std::move(cb); }
    void setCloseCallback(Callback cb) { _closeCallback = std::move(cb); }

    // --- 底层属性 ---
    [[nodiscard]] int getFd() const noexcept { return _fd; }
    [[nodiscard]] uint32_t getEvents() const noexcept { return _events; }
    void setRevents(uint32_t rev) { _revents = rev; }//给Epoll 用的
    [[nodiscard]] bool isInEpoll() const noexcept { return _inEpoll; }
    [[nodiscard]] bool isReading() const noexcept { return _events & EPOLLIN; }
    [[nodiscard]] bool isWriting() const noexcept { return _events & EPOLLOUT; }
    [[nodiscard]] bool isNoneEvent() const noexcept { return _events == 0; }
    void setInEpoll(bool in = true) { _inEpoll = in; }

private:    
    void update(); // 内部辅助：同步当前状态到 Epoll 树上

    Epoll* _ep;
    const int _fd;
    
    bool _inEpoll{false}; //默认不在epoll树上

    uint32_t _events{0}; //用户感兴趣的事件（注册给内核）
    uint32_t _revents{0}; //实际发生的事件（内核返回的）

    Callback _readCallback;//读回调
    Callback _writeCallback;//写回调
    Callback _errorCallback;//错误回调
    Callback _closeCallback;//关闭回调
};