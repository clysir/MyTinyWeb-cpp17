#pragma once

#include "EventLoopThread.h"
#include "EventLoop.h"
#include <vector>
#include <memory>

class EventLoopThreadPool 
{
public:
    EventLoopThreadPool(EventLoop* baseLoop, int threadNums);
    void start();
    EventLoop* getNextLoop();
     
private:
    EventLoop* _baseLoop;
    int _threadNums{0};
    std::vector<std::unique_ptr<EventLoopThread>> _threads;
    std::vector<EventLoop*> _loops;
    int _next{0};
    bool _started{false};
};