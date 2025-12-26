#include "../../include/Http/HttpServer.h"
#include <iostream>

HttpServer::HttpServer(EventLoop* loop, const InetAddress& localAddr, const std::string& name) 
            : _server{loop, localAddr}             
{   // 设置连接回调 → 转发到 HttpServer::onConnection
    _server.setConnectionCallback([this](const TcpConnection::Ptr& conn) {
        onConnection(conn); 
    });
    // 设置消息回调 → 转发到 HttpServer::onMessage
    _server.setMessageCallback([this](const TcpConnection::Ptr& conn, Buffer* buf) {
        onMessage(conn, buf); 
    });
}

void HttpServer::onConnection(const TcpConnection::Ptr& conn) {
    if(conn->connected()) {
        // 来一个客户端 给一个解析上下文
        _contexts[conn->getFd()] = HttpContext();
    }
    else {
        // 客户端断开
        _contexts.erase(conn->getFd());
    }
}

void HttpServer::onMessage(const TcpConnection::Ptr& conn, Buffer* buf) {

    HttpContext& context = _contexts[conn->getFd()];

    std::cout << "[DEBUG] onMessage called, buf size: " << buf->readableBytes() << std::endl;
    std::cout << "[DEBUG] buf content: " << buf->peekAsView() << std::endl;

    // 尝试解析
    context.parseRequest(buf);
    
    std::cout << "[DEBUG] isComplete: " << context.isComplete() << ", hasError: " << context.hasError() << std::endl;
    
    if(context.hasError()) {
        //解析失败 
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
        return;
    }

    // 解析完成 调用用户回调
    if(context.isComplete()) {
        std::cout << "[DEBUG] Request complete, path: " << context.getRequest().getPath() << std::endl;
        
        HttpResponse response;

        if(_httpCallback) _httpCallback(context.getRequest(), &response);

        // 发送响应   ---> 这里是 主函数会设置回调 并且向response里面写入数据  -- 然后我们只需要发送response就行
        std::cout << "[DEBUG] Sending response: " << response.toString() << std::endl;
        conn->send(response.toString());

        // 根据connection 头决定是否关闭
        if(response.isCloseConnection()) {
            conn->shutdown();
        }

        // 重新处理下一个
        context.reset();
    }
}