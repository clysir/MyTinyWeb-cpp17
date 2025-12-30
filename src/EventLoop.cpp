#include "../include/EventLoop.h"
#include "../include/Epoll.h"
#include "../include/Channel.h"
#include "../include/Log/Logger.h"
#include <vector>
#include <sys/eventfd.h>
#include <unistd.h>

EventLoop::EventLoop() 
        : _ep{std::make_unique<Epoll>()},  
        _wakeupFd{eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)},
        _wakeupChannel{std::make_unique<Channel>(_ep.get(), _wakeupFd)}, // &_ep 的类型为 std::unique_ptr<Epoll>*
        _threadId{std::this_thread::get_id()}  
{
    _wakeupChannel->setReadCallback([this] () { handleWakeup(); });
    _wakeupChannel->enableReading();
}

void EventLoop::loop() {
    while(!_quit) {
        std::vector<Channel*> channels;
        channels = _ep->poll();
        for(auto& ch : channels) {
            ch->handleEvent();
        }
        doPendingFunctors();
    }
}

void EventLoop::quit() {
    _quit = true;
    wakeup(); // 唤醒事件循环 此时loop条件不满足 退出循环
}

void EventLoop::updateChannel(Channel* ch) {
    _ep->updateChannel(ch);
}

void EventLoop::removeChannel(Channel* ch) {
    _ep->removeChannel(ch);
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(_wakeupFd, &one, sizeof(one));
    if(n != sizeof(one)) {
        LOG_ERROR << "wakeup() writes " << static_cast<int>(n) << " bytes ";
    }
}

void EventLoop::handleWakeup() {
    uint64_t one;
    ssize_t n = read(_wakeupFd, &one, sizeof(one)); // 会自动清零eventfd
    if(n != sizeof(one)) {
        LOG_ERROR << "handleWakeup() reads " << static_cast<int>(n) << " bytes ";
    }
}

void EventLoop::runInLoop(std::function<void()> cb) {
    if(isInLoopThread()) {
        cb();  // 如果在本线程 直接执行
    }
    else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(std::function<void()> cb) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _pendingFunctors.push_back(std::move(cb));
    }
    if(!isInLoopThread() || _callingPendingFunctors) {
        wakeup();
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<std::function<void()>> functors;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        functors.swap(_pendingFunctors);
    }
    _callingPendingFunctors = true;
    for(auto& cb : functors) {
        cb();
    }
    _callingPendingFunctors = false;
}
