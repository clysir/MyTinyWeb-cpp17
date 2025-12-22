#pragma once

#include "TcpConnection.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include <memory> 
#include <map>
#include <functional>

// 前置声明
class EventLoop;

class Server 
{
public:
    Server(EventLoop* loop, const InetAddress& addr);
    ~Server() = default;
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

public:
    using ConnectionCallback = std::function<void(const TcpConnection::Ptr&)>;
    using MessageCallback = std::function<void(const TcpConnection::Ptr&, Buffer*)>;

    void setConnectionCallback(ConnectionCallback cb) { _connectionCallback = std::move(cb); }
    void setMessageCallback(MessageCallback cb) { _messageCallback = std::move(cb); }

private:
    void handleNewConnection(); 
    void removeConnection(const TcpConnection::Ptr& conn);

private:
    EventLoop* _loop;

    InetAddress _localAddr;

    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Channel> _channel;
    //必须使用shared 不然执行回调时 很可以会报错
    std::map<int, TcpConnection::Ptr> _connection;

    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
};