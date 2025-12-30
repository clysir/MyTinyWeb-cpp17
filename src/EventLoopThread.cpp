#include "../include/EventLoopThread.h"
#include "../include/EventLoop.h"
#include <thread>

// == startLoop 和 threadFunc 相互耦合

EventLoop* EventLoopThread::startLoop() { // 启动子线程，并返回子线程中创建的 EventLoop 指针。
    // 1. 启动子线程，执行 threadFunc
    _thread = std::thread([this] () { threadFunc(); });

    // 2. 等待子线程创建 EventLoop
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.wait(lock, [this] () { return _loop != nullptr; });
        loop = _loop;
    }

    // 3. 返回子线程的 EventLoop 指针
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop; // 这里就是子线程的 EventLoop

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _loop = &loop;
        _cond.notify_one(); // 通知 startLoop
    }
    
    loop.loop(); // 进入事件循环

    // loop 结束后（通常是程序退出时）
    std::lock_guard<std::mutex> lock(_mutex);
    _loop = nullptr;
}

EventLoopThread::~EventLoopThread() {
    if(_loop != nullptr) {
        _loop->quit();
    }
    if(_thread.joinable()) {
        _thread.join(); // 阻塞当前线程，直到 _thread 执行完毕
    }
}