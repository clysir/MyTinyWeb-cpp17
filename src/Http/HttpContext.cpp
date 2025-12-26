#include "../../include/Http/HttpContext.h"
#include "../../include/Http/HttpRequest.h"
#include <algorithm>
#include <cstring>
#include <string>
#include <cctype>

void HttpContext::reset() {
    _state = kExpectRequestLine;
    _request.reset();
    _hasError = false;
    _hasBody = false;
}

bool HttpContext::parseRequest(Buffer* buf) {
    // 有数据 未完成 未出错
    while( buf->readableBytes() > 0 && _state != kDone && !_hasError ) 
    {
        // 从buffer中解析出完整的HttpRequest
        switch(_state) 
        {
            case kExpectRequestLine:
                parseRequestLine(buf);
                break;
            case kExpectHeader:
                parseHeader(buf);
                break;
            case kExpectBody:
                parseBody(buf);
                break;
            default:
                break;
        }
    }
    return _state == kDone;
}

bool HttpContext::parseRequestLine(Buffer* buf) {
    // 请求行格式：GET /path?query HTTP/1.1\r\n

    //先确保有完整的一行
    const char* crlf = buf->findCRLF();
    if(crlf == nullptr) {
        return false;
    }

    const char* begin = buf->peekAsView().data();
    const char* end = crlf; // 只到"\r\n"结束

    // 1. 找第一个空格，分割出 method
    const char* space1 = std::find(begin, end, ' '); 
    // 由于TCP 是流式传输 所以很可能 数据没发完 我们就判断他错误了 ---> 所以需要确保读到了一行数据 
    if(space1 == end) {
        _hasError = true;
        return false;
    }

    std::string method(begin, space1);
    _request.setMethod(method);
    
    // 判断是否是 GET（用于后续决定是否解析 body）
    _hasBody = (method == "POST" || method == "PUT" || method == "DELETE");
    
    // 2. 找第二个空格，分割出 path?query
    const char* space2 = std::find(space1 + 1, end, ' ');
    if(space2 == end) {
        _hasError = true;
        return false;
    }
    
    // 3. 在 path?query 中找 ?，分割 path 和 query
    const char* question = std::find(space1 + 1, space2, '?');
    if(question != space2) {
        // 有 query
        _request.setPath(std::string(space1 + 1, question));
        _request.setQuery(std::string(question + 1, space2));
    } else {
        // 没有 query
        _request.setPath(std::string(space1 + 1, space2));
    }
    
    // 4. version 简化处理，默认 HTTP/1.1
    // 实际的 version 在 space2+1 到 end 之间，但我们直接用 1.1
    
    // 成功，切换到解析 header
    buf->retrieve(crlf + 2 - begin); // 移除这一行 
    _state = kExpectHeader;
    return true;
}

bool HttpContext::parseHeader(Buffer* buf) {
    // 逐行读取 直到遇到空行
    while(1) {
        const char* crlf = buf->findCRLF();
        if(crlf == nullptr) {
            return false;
        }
        const char* start = buf->peekAsView().data();
        // 若这一行都是空行 头部结束
        if(crlf == start) {
            buf->retrieve(2);
            if(_hasBody) { // 只有请求为POST 时才进入body
                _state = kExpectBody;
                return true;
            }
            else {
                _state = kDone;
                return true;
            }
        }
        // 找冒号
        const char* colon = std::find(start, crlf, ':'); //crlf 就是每一行的末尾 "\r\n"
        if(colon == crlf) { //说明未找到 ':'
            _hasError = true;
            return false;
        }
        std::string key(start, colon);
        std::string value(colon + 2, crlf); // 因为实际是 ": " 所以要跳过两个字符
        
        _request.setHeader(key, value);

        buf->retrieve(crlf + 2 - start);
    }

}

bool HttpContext::parseBody(Buffer* buf) {
    // 获取 Content-Length 头部
    const std::string& lengthStr = _request.getHeader("Content-Length"); // 会获得value

    // 如果没有 认为没有Body
    if(lengthStr.empty()) {
        _state = kDone;
        return true;
    }
    
    // 若里面不是数字
    if(!std::all_of(lengthStr.begin(), lengthStr.end(), ::isdigit)) {
        _hasError = true;
        return false;
    }

    // 如果有 解析body长度
    size_t bodyLength = std::stoul(lengthStr);

    // 限制大小
    if(bodyLength > 10 * 1024 * 1024) { // 10MB
        _hasError = true;
        return false;
    }

    // 检查数据是否收完 
    if(buf->readableBytes() < bodyLength) {
        // 数据还没收完，等待更多数据
        // 下次 parseRequest 会再次进入这里
        return false;
    }
    // 数据收完，设置body
    _request.setBody(std::string(buf->peekAsView().data(), bodyLength));
    buf->retrieve(bodyLength);

    _state = kDone;
    return true;
}