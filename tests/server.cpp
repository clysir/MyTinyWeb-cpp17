#include "../include/Server.h"
#include "../include/EventLoop.h"
#include <iostream>

int main() {
    EventLoop loop;
    InetAddress localAddr("127.0.0.1", 8008);
    Server server(&loop, localAddr);

    server.setMessageCallback([](const TcpConnection::Ptr& conn, Buffer* buf) {
        std::string msg = buf->retrieveAllAsString();
        std::cout << "Recv: " << msg << std::endl;
        conn->send("Echo " + msg);
    });
    loop.loop();
    return 0;
}


