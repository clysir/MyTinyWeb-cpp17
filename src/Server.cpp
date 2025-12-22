#include "../include/Server.h"
#include "../include/Channel.h"
#include "../include/EventLoop.h"
#include "../include/Socket.h"
#include <iostream>
#include <unistd.h>

Server::Server(EventLoop* loop, const InetAddress& addr) 
            : _loop{loop}, _localAddr{addr},
              _socket{std::make_unique<Socket>()},
              _channel{std::make_unique<Channel>(_loop->getEpoll(), _socket->getFd())}
{
    _socket->bindAddress(addr);
    _socket->listen();
    _socket->setNonBlocking();
    
    //只设置读回调
    _channel->setReadCallback([this]() { handleNewConnection(); });
    _channel->enableReading();
}

void Server::handleNewConnection() {
    //处理连接
    InetAddress peerAddr;
    int connfd = _socket->accept(peerAddr);
    if(connfd < 0) return; 
    std::cout << "New Connection form Client:[" << connfd << "] " << std::endl;

    //创建一个TcpConnection
    auto conn = std::make_shared<TcpConnection>(_loop, connfd, _localAddr, peerAddr);
    
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);

    conn->setCloseCallback([this](const TcpConnection::Ptr& conn) { removeConnection(conn); });
    //保存连接
    _connection[connfd] = conn;

    //激活连接
    conn->connectEstablished();
}

void Server::removeConnection(const TcpConnection::Ptr& conn) {
    _connection.erase(conn->getFd());
    conn->connectDestroyed();
}







