#include "../include/Server.h"
#include "../include/Channel.h"
#include "../include/EventLoop.h"
#include "../include/Socket.h"
#include <iostream>
#include <unistd.h>

Server::Server(EventLoop* loop, const InetAddress& addr) {
    _loop = loop;

    _socket = std::make_unique<Socket>();
    _socket->Bind(addr);
    _socket->Listen();
    _socket->SetNonBlocking();
    
    _channel = std::make_unique<Channel>(_loop->getEpoll(), _socket->GetFd());
    //_channel->setReadCallback(std::bind(&Server::handleNewConnection, this));
    _channel->setReadCallback([this]() { handleNewConnection(); });
    _channel->enableReading();
}

void Server::handleNewConnection() {
    InetAddress client_addr;
    int client_fd = _socket->Accept(client_addr);
    std::cout << "New Connection form Client:[" << client_fd << "] " << std::endl;

    _client_sockets[client_fd] = std::make_unique<Socket>(client_fd); 
    _client_channels[client_fd] = std::make_unique<Channel>(_loop->getEpoll(), client_fd);

    //设置Channel的回调
    //_client_channels[client_fd]->setReadCallback(std::bind(&Server::handleEvent, this, client_fd));
    _client_channels[client_fd]->setReadCallback([this, client_fd]() { handleEvent(client_fd); });
    //开启监听
    _client_channels[client_fd]->enableReading();
}

void Server::handleEvent(int fd) {
    //目前是实现echo 我们接收到什么 就发送回去
    char buffer[1024];
    int read_byte = read(fd, buffer, sizeof(buffer) - 1);
    if (read_byte > 0) {
        buffer[read_byte] = '\0';
        std::cout << "Recv: " << buffer << std::endl;
        write(fd, buffer, read_byte);
    }
    else if (read_byte == 0) {
        std::cout << "Client is disconnect " << std::endl;
        _client_channels.erase(fd);
        _client_sockets.erase(fd);
        //close(fd);
    }
    else {
        perror("handleEvent failed");
        _client_channels.erase(fd); //智能指针会自动析构 close掉fd
        _client_sockets.erase(fd);
        //close(fd);
    }
}

Server::~Server() {

}