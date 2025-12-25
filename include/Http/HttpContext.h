#pragma once

#include "../Buffer.h"
#include "HttpRequest.h"
// ====== 从buffer中解析出完整的 HttpRequest ========

class HttpContext
{
public:
    enum ParseState
    {
        kExpectRequestLine,
        kExpectHeader,
        kExpectBody,
        kDone
    };

public:
    // 从Buffer解析 成功返回true
    [[nodiscard]] bool parseRequest(Buffer* buf);

    // 状态查询
    [[nodiscard]] bool isComplete() const { return _state == kDone; }
    [[nodiscard]] bool hasError() const { return _hasError; }

    // 获取请求
    [[nodiscard]] const HttpRequest& getRequest() const { return _request; }
    [[nodiscard]] HttpRequest& getRequest() { return _request; }

    // 重置
    void reset();

private:
    // 内部解析函数
    bool parseRequestLine(Buffer* buf);
    bool parseHeader(Buffer* buf);
    bool parseBody(Buffer* buf);

private:
    ParseState _state{kExpectRequestLine};
    HttpRequest _request;
    bool _hasError{false};
    bool _hasBody{false};
};