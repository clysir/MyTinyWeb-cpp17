# MyTinyWeb

一个基于 **Reactor 模式** 的轻量级 C++ 高性能网络服务器。本项目旨在从底层 Socket 编程开始，逐步构建一个支持高并发、具备现代 C++ 特性的 Web 服务器。

## 🚀 当前版本功能 (V3.0 - 静态文件服务)

### ✅ 新增静态文件服务
- **MimeType**：根据文件后缀自动识别 Content-Type（html/css/js/png/jpg 等）
- **静态文件服务**：支持从 `www/` 目录读取 HTML、CSS、JS、图片等资源
- **多页面支持**：通过 `<a href>` 实现页面跳转

### ✅ HTTP 模块
- **HttpRequest**：存储 HTTP 请求信息（method, path, query, headers, body）
- **HttpResponse**：构建 HTTP 响应，自动添加 Content-Length
- **HttpContext**：状态机解析器，支持分块数据接收
- **HttpServer**：HTTP 服务器封装，提供简洁的回调 API

### ✅ 核心组件
- **Socket**：RAII 封装，支持 `SO_REUSEADDR/SO_REUSEPORT`
- **InetAddress**：IP 地址与端口的封装
- **Epoll**：使用 `std::vector` 动态管理事件，支持 `EPOLL_CLOEXEC`
- **Channel**：事件分发器，支持读/写/关闭/错误回调
- **Buffer**：双游标设计，支持 `readv` 高性能读取和动态扩容
- **TcpConnection**：连接管理器，封装 Socket + Channel + Buffer
- **EventLoop**：事件循环核心，驱动整个 Reactor 模式
- **Server**：服务器入口，管理所有 TcpConnection

### ✅ 现代 C++17 特性
- `std::unique_ptr` / `std::shared_ptr` 智能指针
- `std::string_view` 零拷贝字符串视图
- `[[nodiscard]]`、`noexcept` 属性
- Lambda 表达式与 `std::function` 回调
- `std::enable_shared_from_this` 生命周期管理

## 🏗️ 架构设计

```
                              ┌────────────────────────────────┐
                              │         HttpServer             │
                              │   (HTTP 协议层，回调式 API)      │
                              └───────────────┬────────────────┘
                                              │ composes
                              ┌───────────────▼────────────────┐
                              │            Server              │
                              │   (监听Socket + 连接管理)        │
                              └───────────────┬────────────────┘
                                              │ creates
                              ┌───────────────▼────────────────┐
                              │        TcpConnection           │
                              │   (Socket + Channel + Buffer)   │
                              └───────────────┬────────────────┘
                                              │ registers
      ┌────────────────┬──────────────────────▼────┬────────────────┐
      │     Channel    │          Epoll            │     Buffer     │
      │    (事件开关)   │       (事件监听心脏)        │    (数据缓冲)   │
      └────────────────┴───────────────────────────┴────────────────┘
```

## 🛠️ 如何编译与运行

### 依赖环境
- Linux 系统 (由于使用了 Epoll)
- CMake (3.10+)
- 支持 C++17 的编译器 (如 GCC 7+)

### 编译步骤
```bash
cd tests
mkdir build && cd build
cmake ..
make
```

### 运行静态文件服务器
```bash
# 终端 1: 从项目根目录启动（重要！）
cd ~/MyTinyWeb
./tests/build/http_web

# 终端 2: 浏览器访问
# http://localhost:8008/           (首页)
# http://localhost:8008/test.html   (其他页面)
# http://localhost:8008/css/style.css (样式文件)
```

## 📅 下一步计划
- [x] ~~实现静态文件服务（从磁盘读取 HTML/CSS/JS）~~ ✅ V3.0 已完成
- [ ] POST 请求解析（表单提交、API 接口）
- [ ] Multi-Reactor 模式（One Loop Per Thread）
- [ ] 引入线程池处理业务逻辑
- [ ] 定时器 Timer 支持
- [ ] 日志系统

---
*Created by [chenly](https://github.com/chenly)*
