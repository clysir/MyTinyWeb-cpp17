#pragma once

#include "InetAddress.h"
#include <memory> 
#include <map>

// 前置声明
class EventLoop;
class Socket;
class Channel;

class Server 
{
public:
    Server(EventLoop* loop, const InetAddress& addr);
    ~Server();

private:
    void handleNewConnection(); 
    void handleEvent(int fd); 
private:
    EventLoop* _loop;
    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Channel> _channel;

    // 用两个 map 来管理所有客户端的 Socket 和 Channel
    // Key 是文件描述符 fd
    std::map<int, std::unique_ptr<Socket>> _client_sockets;
    std::map<int, std::unique_ptr<Channel>> _client_channels;
};