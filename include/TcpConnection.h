#pragma once

#include <memory>
#include <functional>
#include <string>
#include <string_view>
#include "InetAddress.h"
#include "Buffer.h"
#include "Socket.h"
#include "Channel.h"

class EventLoop;

/*
* TcpConnection 封装了一个已建立的客户端连接
* 包含 Socket, Channel, 以及对应的 读写缓冲区
*/
class TcpConnection : public std::enable_shared_from_this<TcpConnection> 
{
public:
    using Ptr = std::shared_ptr<TcpConnection>;//类型宏定义
    // 回调函数定义
    using ConnectionCallback = std::function<void(const Ptr&)>;
    using MessageCallback = std::function<void(const Ptr&, Buffer*)>;
    using CloseCallback = std::function<void(const TcpConnection::Ptr&)>;

    TcpConnection(EventLoop* loop,int fd, const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection() = default;

    //业务接口
    void send(std::string_view msg);
    void shutdown(); // 写端关闭

    // 设置由 Server 提供的业务回调
    void setConnectionCallback(ConnectionCallback cb) { _connectionCallback = std::move(cb); }
    void setMessageCallback(MessageCallback cb) { _messageCallback = std::move(cb); }
    void setCloseCallback(CloseCallback cb) { _closeCallback = std::move(cb); }

    //连接状态控制 （供Server 调用）
    void connectEstablished(); //建立连接
    void connectDestroyed(); //销毁连接

    [[nodiscard]] int getFd() const noexcept { return _socket->getFd(); }

private:
    // 由 Channel 触发的底层回调
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

private:
    //状态机
    enum StateE {
        kConnecting,
        kConnected,
        kDisconnecting,
        kDisconnected
    };
    void setState(StateE s) { _state = s; }

private:
    EventLoop* _loop;
    StateE _state;
    
    const InetAddress _localAddr;
    const InetAddress _peerAddr;

    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Channel> _channel;

    Buffer _inputBuffer;  // 读缓冲区 内核写，程序读
    Buffer _outputBuffer; // 写缓冲区 程序写，内核读

    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    CloseCallback _closeCallback;
};