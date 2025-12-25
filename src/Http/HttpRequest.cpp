#include "../../include/Http/HttpRequest.h"

// 把所有成员恢复到初始状态，准备解析下一个请求。
void HttpRequest::reset() {
    _method = Method::kInvalid;
    _version = Version::kUnknown;
    _path.clear();
    _query.clear();
    _headers.clear();
    _body.clear();
}
    
const std::string& HttpRequest::getMethodString() const noexcept {
    static const std::string methodString[] = {
        "INVALID",
        "GET",
        "POST",
        "HEAD",
        "PUT",
        "DELETE"
    };
    return methodString[_method];
}
// 输入key 返回value
const std::string& HttpRequest::getHeader(const std::string& field) const noexcept {
    static const std::string empty;
    auto it = _headers.find(field);
    return (it != _headers.end()) ? it->second : empty;
}

void HttpRequest::setMethod(const std::string& method) {
    if(method == "GET") _method = kGet;
    else if(method == "POST") _method = kPost;
    else if(method == "HEAD") _method = kHead;
    else if(method == "PUT") _method = kPut;
    else if(method == "DELETE") _method = kDelete;
    else _method = kInvalid;
}

// 本项目其实没有使用这里的逻辑 算是一个预留的接口
void HttpRequest::setVersion(const std::string& version) {
    if(version == "HTTP/1.1") _version = kHttp11;
    else if(version == "HTTP/1.0") _version = kHttp10;
    else _version = kUnknown;
}

