#include "../include/Http/HttpServer.h"
#include "../include/Http/MimeType.h"
#include "../include/EventLoop.h"
#include "../include/InetAddress.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

int main() {
    EventLoop loop;
    InetAddress addr("127.0.0.1", 8008);
    HttpServer server(&loop, addr);
    
    server.setHttpCallback([](const HttpRequest& req, HttpResponse* resp){
        std::string path = req.getPath();
        // 修改原始路径
        if(path == "/") path = "/index.html";

        // 拼接文件
        std::string filePath = "./www" + path; // ./www/index.html
        
        // 读取文件
        std::ifstream file(filePath);
        
        if(file) {
            std::stringstream content;
            content << file.rdbuf();

            resp->setStatusCode(HttpResponse::k200OK);
            resp->setContentType(MimeType::fromPath(filePath));
            resp->setBody(content.str());
        } else {
            resp->setStatusCode(HttpResponse::k404NotFound);
            resp->setBody("404 Not Found");
        }
    }); 

    loop.loop();
    return 0;
}
