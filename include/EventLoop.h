#pragma once

// ==== 这是主线程的loop 主要用来accept 并将其交给一个任意的thread =======

#include "Epoll.h"
#include "Channel.h"
#include <memory>
#include <mutex>
#include <functional>
#include <thread>
#include <vector>

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

public:
    // 唤醒
    void wakeup(); 
    void handleWakeup();

    // 跨线程投递任务
    void runInLoop(std::function<void()> cb);
    void queueInLoop(std::function<void()> cb);

    // 检查是否在当前线程
    [[nodiscard]] bool isInLoopThread() const noexcept { return _threadId == std::this_thread::get_id(); }

    // 执行处理任务
    void doPendingFunctors();

private:
    std::unique_ptr<Epoll> _ep;
    bool _quit{false}; // 标记是否退出

private:
    int _wakeupFd; // 用来唤醒阻塞的 epoll_wait
    std::unique_ptr<Channel> _wakeupChannel; // 监听wakeupFd的可读事件
    std::mutex _mutex;
    std::thread::id _threadId; // 记录当前线程ID
    std::vector<std::function<void()>> _pendingFunctors; // 待执行的任务 
    bool _callingPendingFunctors{false}; // 是否在执行任务
};
