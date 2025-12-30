#include "../include/EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, int threadNums) 
                    :_baseLoop{baseLoop},
                    _threadNums{threadNums > 0 ? threadNums : static_cast<int>(std::thread::hardware_concurrency())} {}

void EventLoopThreadPool::start() {
    _started = true;
    for(int i = 0; i < _threadNums; i++) {
        auto t = std::make_unique<EventLoopThread>();
        _loops.push_back(t->startLoop());
        _threads.push_back(std::move(t));
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    if(_loops.empty()) {
        return _baseLoop; // 没有子线程 返回主线程loop
    }
    EventLoop* loop = _loops[_next];
    _next = (_next + 1) % _threadNums;
    return loop;
}
