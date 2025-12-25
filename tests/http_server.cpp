#include "../include/Http/HttpServer.h"
#include "../include/EventLoop.h"
#include "../include/InetAddress.h"
#include <iostream>

// ===== 只实现了静态 ====

int main() {
    EventLoop loop;
    InetAddress localAddr("127.0.0.1", 8008);
    HttpServer server(&loop, localAddr);

    server.setHttpCallback([](const HttpRequest& req, HttpResponse* resp) {
        if(req.getPath() == "/") {
            resp->setStatusCode(HttpResponse::k200OK);
            resp->setContentType("text/html");
            resp->setBody("<h1>Welcome to MyTinyWeb!</h1>");
        }
        else if(req.getPath() == "/hello") {
            resp->setStatusCode(HttpResponse::k200OK);
            resp->setContentType("text/plain");
            resp->setBody("Hello, World!");
        }
        else {
            resp->setStatusCode(HttpResponse::k404NotFound);
            resp->setBody("404 Not Found");
        }
    });
    
    loop.loop();
    return 0;
}