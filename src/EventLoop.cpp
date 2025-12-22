#include "../include/EventLoop.h"
#include "../include/Epoll.h"
#include "../include/Channel.h"
#include <vector>

EventLoop::EventLoop() : _ep{std::make_unique<Epoll>()} {}

void EventLoop::loop() {
    while(!_quit) {
        std::vector<Channel*> channels;
        channels = _ep->poll();
        for(auto& ch : channels) {
            ch->handleEvent();
        }
    }
}

void EventLoop::quit() {
    _quit = true;
}

void EventLoop::updateChannel(Channel* ch) {
    _ep->updateChannel(ch);
}

void EventLoop::removeChannel(Channel* ch) {
    _ep->removeChannel(ch);
}

