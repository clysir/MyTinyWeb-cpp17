#include "../include/Server.h"
#include "../include/EventLoop.h"
#include "../include/InetAddress.h"
#include <iostream>

int main() {
    EventLoop* loop = new EventLoop();
    InetAddress addr("127.0.0.1", 8008);
    Server server(loop, addr);

    loop->loop();
    return 0;
}