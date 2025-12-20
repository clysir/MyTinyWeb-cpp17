#include "../include/EventLoop.h"
#include "../include/Epoll.h"
#include "../include/Channel.h"
#include <vector>

EventLoop::EventLoop() : _ep(new Epoll()), _quit(false) {}

void EventLoop::loop() {
    while(!_quit) {
        std::vector<Channel*> channels;
        channels = _ep->poll();
        for(auto& ch : channels) {
            ch->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* ch) {
    _ep->updateChannel(ch);
}

EventLoop::~EventLoop() { delete _ep; }