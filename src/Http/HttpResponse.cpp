#include "../../include/Http/HttpResponse.h"

void HttpResponse::setStatusCode(StatusCode code) {
    _statusCode = code;
    
    switch(_statusCode) 
    {
        case k200OK:
            _statusMessage = "OK";
            break;
        case k301MovedPermanently:
            _statusMessage = "Moved Permanently";
            break;
        case k400BadRequest:
            _statusMessage = "Bad Request";
            break;
        case k404NotFound:
            _statusMessage = "Not Found";
            break;
        case k500InternalServerError:
            _statusMessage = "Internal Server Error";
            break;
        default:
            _statusMessage = "Unknown";
            break;
    }
}

std::string HttpResponse::toString() const {
    std::string response;
    //状态行
    response += "HTTP/1.1 " + std::to_string(_statusCode) + " " + _statusMessage + "\r\n";
    
    //头部
    for (const auto& header : _headers) {
        response += header.first + ": " + header.second + "\r\n";
    }
    
    // Content-Type
    if(!_contentType.empty()) {
        response += "Content-Type: " + _contentType + "\r\n";
    }
    
    // Content-Length（必须有！否则 curl 不知道响应何时结束）
    response += "Content-Length: " + std::to_string(_body.size()) + "\r\n";
    
    // 关闭连接
    if(_closeConnection) {
        response += "Connection: close\r\n";
    }
    
    //空行
    response += "\r\n";
    //正文
    response += _body;
    return response;
}
