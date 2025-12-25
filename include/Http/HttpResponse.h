#pragma once

#include <string>
#include <map>
/*************** 构建HTTP响应 生成响应字符串 ********/

class HttpResponse
{
public:
    enum StatusCode 
    {
        k200OK = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,
        k500InternalServerError = 500, 
    };

public:
    // 接口
    void setStatusCode(StatusCode code);
    void setStatusMessage(const std::string& msg) { _statusMessage = msg; }
    void setContentType(const std::string& type) { _contentType = type; }
    void addHeader(const std::string& key, const std::string& value) { _headers[key] = value; }
    void setBody(const std::string& body) { _body = body; }
    void setCloseConnection(bool close) { _closeConnection = close; }

    // 查询
    [[nodiscard]] bool isCloseConnection() const { return _closeConnection; }
    
    // 生成响应字符串
    std::string toString() const;

private:
    StatusCode _statusCode{k200OK};
    std::string _statusMessage;
    std::string _contentType;
    std::map<std::string, std::string> _headers;
    std::string _body;
    bool _closeConnection{false};
};