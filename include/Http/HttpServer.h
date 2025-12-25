#pragma once

#include "../EventLoop.h"
#include "../InetAddress.h"
#include "../Server.h"
#include "../TcpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpContext.h"
#include <functional>
#include <map>

class HttpServer
{
public:
    HttpServer(EventLoop* loop, const InetAddress& localAddr, const std::string& name = "HttpServer");
    ~HttpServer() = default;

public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>; //request 一定有 response不一定有
    void setHttpCallback(HttpCallback cb) { _httpCallback = std::move(cb); }

private:
    // 内部回调： 连接建立
    void onConnection(const TcpConnection::Ptr& conn);

    // 内部回调 消息处理
    void onMessage(const TcpConnection::Ptr& conn, Buffer* buf);

private:
    Server _server;
    HttpCallback _httpCallback;
    std::map<int, HttpContext> _contexts; // fd -> 解析上下文
};