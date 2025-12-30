#include "../../include/Http/HttpServer.h"
#include "../../include/Log/Logger.h"

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
        conn->setContext(HttpContext()); // 这里会默认创建一个HC对象 都是初始条件
    }
    // 断开时 conn析构会自动清理
}

void HttpServer::onMessage(const TcpConnection::Ptr& conn, Buffer* buf) {

    HttpContext& context = std::any_cast<HttpContext&>(conn->getContext());

    // LOG_DEBUG << "onMessage called, buf size: " << buf->readableBytes();
    // LOG_DEBUG << "buf content: " << buf->peekAsView();

    // 尝试解析
    context.parseRequest(buf);
    
    // LOG_DEBUG << "isComplete: " << context.isComplete() << ", hasError: " << context.hasError();
    
    if(context.hasError()) {
        //解析失败 
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
        return;
    }

    // 解析完成 调用用户回调
    if(context.isComplete()) {
        // LOG_DEBUG << "Request complete, path: " << context.getRequest().getPath();
        
        HttpResponse response;

        if(_httpCallback) _httpCallback(context.getRequest(), &response);

        // 发送响应   ---> 这里是 主函数会设置回调 并且向response里面写入数据  -- 然后我们只需要发送response就行
        // LOG_DEBUG << "Sending response: " << response.toString();
        conn->send(response.toString());

        // 根据connection 头决定是否关闭
        if(response.isCloseConnection()) {
            conn->shutdown();
        }

        // 重新处理下一个
        context.reset();
    }
}