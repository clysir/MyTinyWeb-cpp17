#pragma once

#include "EventLoop.h"
#include <mutex>
#include <thread>
#include <condition_variable>

class EventLoopThread
{
public:
    EventLoopThread() = default;
    ~EventLoopThread();

    EventLoop* startLoop(); // 启动线程并返回该线程的 EventLoop*

private:
    void threadFunc(); // 线程入口函数
    EventLoop* _loop;
    std::thread _thread;
    std::mutex _mutex;
    std::condition_variable _cond;
};