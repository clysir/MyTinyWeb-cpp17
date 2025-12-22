#include "../include/TcpConnection.h"
#include "../include/Socket.h"
#include "../include/Channel.h"
#include "../include/EventLoop.h"
#include <unistd.h>
#include <iostream>

TcpConnection::TcpConnection(EventLoop* loop, int fd, const InetAddress& local, const InetAddress& peer) 
    : _loop{loop}, _state{kConnecting}, _localAddr{local}, _peerAddr{peer},
    _socket{std::make_unique<Socket>(fd)},
    _channel{std::make_unique<Channel>(_loop->getEpoll(), fd)}
{
    _channel->setReadCallback ([this]() { handleRead(); });
    _channel->setWriteCallback([this]() { handleWrite(); });
    _channel->setCloseCallback([this]() { handleClose(); });
    _channel->setErrorCallback([this]() { handleError(); });
}

void TcpConnection::handleRead() {
    int saveErrno = 0;
    //利用Buffer里面的readFd
    ssize_t n = _inputBuffer.readFd(_socket->getFd(), &saveErrno);
    if(n > 0) {
        //读到数据 调业务回调
        _messageCallback(shared_from_this(), &_inputBuffer);
    } else if(n == 0) {
        handleClose();
    } else {
        if(errno != EAGAIN && errno != EWOULDBLOCK) {
            handleError();
        }
    }
}

void TcpConnection::send(std::string_view msg) {
    if(_state != kConnected) return;
        
    size_t nwrote = 0;
    size_t remaining = msg.size();
    bool faultError = false;

    // 尝试直接写
    if(!_channel->isWriting() && _outputBuffer.readableBytes() == 0) {
        nwrote = ::write(_socket->getFd(), msg.data(), msg.size());
        if(nwrote >= 0) {
            remaining = msg.size() - nwrote;
            if(remaining == 0) {
                //这里以后在补充逻辑 这里已经 Mission Complete
            }
        }
        else {
            nwrote = 0;
            if(errno != EWOULDBLOCK) faultError = true; 
        }
    }
    // 写不完 
    if(!faultError && remaining > 0) {
        _outputBuffer.append(msg.substr(nwrote));
        if(!_channel->isWriting()) {
            _channel->enableWriting();// 开启写监听，等 handleWrite 调用
        }
    }
}

void TcpConnection::handleWrite() {
    ssize_t n = 0;
    if(_channel->isWriting()) {
        n = ::write(_socket->getFd(), _outputBuffer.peekAsView().data(), _outputBuffer.readableBytes());
    }
    if(n > 0) {
        _outputBuffer.retrieve(n);
        if(_outputBuffer.readableBytes() == 0) {
            _channel->disableWriting(); // 发完了，关闭写监听
            //这里可以调用一个 writeCompleteCallback
            if(_state == kDisconnecting) {
                ::shutdown(_socket->getFd(), SHUT_WR);
            }
        }
    }
    
}

void TcpConnection::handleClose() {
    if(_state == kConnected || _state == kDisconnecting) {
        setState(kDisconnected);

        _channel->disableAll();
        
        TcpConnection::Ptr guardthis(shared_from_this());
        if(_connectionCallback) _connectionCallback(guardthis);
        if(_closeCallback) _closeCallback(shared_from_this());
    }
}

void TcpConnection::handleError() {
    int optval;
    socklen_t optlen = sizeof(optval);
    if(::getsockopt(_socket->getFd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) == 0) {
        std::cerr << "TcpConnection::handleError - SO_ERROR: " << optval << std::endl;
    }
    handleClose();
}

void TcpConnection::shutdown() {
    if(_state == kConnected) {
        setState(kDisconnecting);
       if(_outputBuffer.readableBytes() == 0) {
            // 没有积压数据，直接关闭写端
            ::shutdown(_socket->getFd(), SHUT_WR);
       }
    }
}

void TcpConnection::connectEstablished() {
    setState(kConnected);
    _channel->enableReading();
    if(_connectionCallback) _connectionCallback(shared_from_this());
}

//显式删除
void TcpConnection::connectDestroyed() {
    if(_state == kConnected) {
        setState(kDisconnected);
        _channel->disableAll();
        if(_connectionCallback) _connectionCallback(shared_from_this());
    }
}