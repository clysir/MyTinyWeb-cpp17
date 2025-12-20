#pragma once

#include <functional>
#include <cstdint>

class Epoll;

class Channel
{
public:
    // 定义回调函数的类型：void() 不需要参数
    using Callback = std::function<void()>;

    Channel(Epoll* ep,int fd);
    ~Channel();

public:
    // 核心功能 1: 开启监听
    void enableReading();
    void setReadCallback(Callback cb) { _readCallback = cb; }
    // 这个函数内部判断是 读事件 还是 写事件，然后调对应的 _callback
    void handleEvent(); 
    int getFd() const { return _fd; }
    uint32_t getEvents() const { return _events; }
    uint32_t getRevents() const { return _revents; }
    void setRevents(uint32_t rev) { _revents = rev; }//给Epoll 用的
    bool isInEpoll() const { return _inEpoll; }
    void setInEpoll(bool in = true) { _inEpoll = in; }
    
private:    
    Epoll* _ep;
    int _fd;
    
    bool _inEpoll = false;//默认不在epoll树上

    uint32_t _events;
    uint32_t _revents;//实际发生的事件

    Callback _readCallback;//读回调
    Callback _writeCallback;//写回调
};