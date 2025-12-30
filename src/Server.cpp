#include "../include/Server.h"
#include "../include/Channel.h"
#include "../include/EventLoop.h"
#include "../include/Socket.h"
#include "../include/Log/Logger.h"
#include <unistd.h>

Server::Server(EventLoop* loop, const InetAddress& addr, int threadNums) 
            : _loop{loop}, _localAddr{addr}, 
              _socket{std::make_unique<Socket>()},
              _channel{std::make_unique<Channel>(_loop->getEpoll(), _socket->getFd())},
              _threadPool{std::make_unique<EventLoopThreadPool>(loop, threadNums)}
{
    _threadPool->start();

    _socket->bindAddress(addr);
    _socket->listen();
    _socket->setNonBlocking();

    //只设置读回调 ----> 客户端每一次连接都会触发一次回调 
    _channel->setReadCallback([this]() { handleNewConnection(); });
    _channel->enableReading();
}

void Server::handleNewConnection() {
    //处理连接
    InetAddress peerAddr;
    int connfd = _socket->accept(peerAddr);
    if(connfd < 0) return; 
    LOG_INFO << "New Connection from Client:[" << connfd << "]";

    // 选一个子线程
    EventLoop* ioLoop = _threadPool->getNextLoop();

    // 主线程创建连接 但是不激活
    auto conn = std::make_shared<TcpConnection>(ioLoop, connfd, _localAddr, peerAddr); //注意 conn里面的_loop 就是子线程的loop  ---- 注意和server构造时的_loop区分 -- 这里的是主线程的
    conn->setConnectionCallback(_connectionCallback);//  _connectionCallback  这里传递的是HttpServer设置的回调 即 onConnection
    conn->setMessageCallback(_messageCallback);//_messageCallback  这里传递的是HttpServer设置的回调 即 onMessage

    conn->setCloseCallback([this](const TcpConnection::Ptr& c) { removeConnection(c); });
    
    // 主线程保存连接
    _connection[connfd] = conn;

    // 投递到子线程激活连接
    ioLoop->runInLoop([conn]() {
        conn->connectEstablished();
    }); 
}

// 子线程触发 投递到主线程
void Server::removeConnection(const TcpConnection::Ptr& conn) {
    _loop->runInLoop([this, conn]() { // 这里的_loop是主线程的EventLoop  因为这是在server.cpp里面写的 有[this]
        removeConnectionInLoop(conn);
    });
}

void Server::removeConnectionInLoop(const TcpConnection::Ptr& conn) {
    _connection.erase(conn->getFd()); // 主线程操作

    // 在投递回子线程 执行 connectDestroyed 
    EventLoop* ioLoop = conn->getLoop(); // 这里获取的ioLoop 因为构造conn时 传入的就是ioLoop
    ioLoop->runInLoop([conn]() {
        conn->connectDestroyed();
    });
}







