#pragma once

/****  此文件用于存储http请求信息 *****/
#include <string>
#include <map>

class HttpRequest
{
public:
    enum Method
    {
        kInvalid,
        kGet,
        kPost,
        kHead,
        kPut,
        kDelete
    };
    enum Version
    {
        kUnknown,
        kHttp10,
        kHttp11
    };
    
public:
    // 设置接口 给context调用
    void setMethod(const std::string& method);
    void setPath(const std::string& path) { _path = path; }
    void setQuery(const std::string& query) { _query = query; }
    void setHeader(const std::string& key, const std::string& value) { _headers[key] = value; }
    void setBody(const std::string& body) { _body = body; }
    void setVersion(const std::string& version);

    // 获取接口 供回调使用
    [[nodiscard]] Method getMethod() const noexcept { return _method; }
    [[nodiscard]] Version getVersion() const noexcept { return _version; }
    [[nodiscard]] const std::string& getMethodString() const noexcept;
    [[nodiscard]] const std::string& getPath() const noexcept { return _path; }
    [[nodiscard]] const std::string& getQuery() const noexcept { return _query; }
    [[nodiscard]] const std::map<std::string, std::string>& getHeaders() const noexcept { return _headers; }
    [[nodiscard]] const std::string& getHeader(const std::string& field) const noexcept;//提供一个按字段查询的接口
    [[nodiscard]] const std::string& getBody() const noexcept { return _body; }
    
    //重置接口
    void reset();
    
private:
    Method _method{kInvalid};
    Version _version{kUnknown};
    std::string _path;
    std::string _query;
    std::map<std::string, std::string> _headers; //由于header本身就是一个键值对的形式
    std::string _body;
};