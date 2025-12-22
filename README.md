# MyTinyWeb

一个基于 **Reactor 模式** 的轻量级 C++ 高性能网络服务器。本项目旨在从底层 Socket 编程开始，逐步构建一个支持高并发、具备现代 C++ 特性的 Web 服务器。

## 🚀 当前版本功能 (V2.0 - TcpConnection 阶段)

### ✅ 核心组件
- **Socket**：RAII 封装，支持 `SO_REUSEADDR/SO_REUSEPORT`
- **InetAddress**：IP 地址与端口的封装
- **Epoll**：使用 `std::vector` 动态管理事件，支持 `EPOLL_CLOEXEC`
- **Channel**：事件分发器，支持读/写/关闭/错误回调
- **Buffer**：双游标设计，支持 `readv` 高性能读取和动态扩容
- **TcpConnection**：连接管理器，封装 Socket + Channel + Buffer，支持状态机和异步发送
- **EventLoop**：事件循环核心，驱动整个 Reactor 模式
- **Server**：服务器入口，管理所有 TcpConnection，提供回调注册接口

### ✅ 现代 C++17 特性
- `std::unique_ptr` / `std::shared_ptr` 智能指针
- `std::string_view` 零拷贝字符串视图
- `[[nodiscard]]`、`noexcept` 属性
- Lambda 表达式与 `std::function` 回调
- `std::enable_shared_from_this` 生命周期管理

## 🏗️ 架构设计

```
                    ┌──────────────────────────────────────┐
                    │              Server                  │
                    │  (监听Socket + TcpConnection管理)     │
                    └──────────────────┬───────────────────┘
                                       │ creates
                    ┌──────────────────▼───────────────────┐
                    │           TcpConnection              │
                    │  (Socket + Channel + 双Buffer)       │
                    └──────────────────┬───────────────────┘
                                       │ registers
        ┌────────────────┬─────────────▼─────────┬────────────────┐
        │     Channel    │        Epoll          │     Buffer     │
        │   (事件开关)    │    (事件监听心脏)      │   (数据缓冲)    │
        └────────────────┴───────────────────────┴────────────────┘
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

### 运行 Echo 服务测试
```bash
# 终端 1: 启动服务器
./server

# 终端 2: 连接测试
nc 127.0.0.1 8008
# 输入任意文字，服务器会回复 Echo: ...
```

## 📅 下一步计划
- [ ] 实现 HTTP 协议解析
- [ ] Multi-Reactor 模式（One Loop Per Thread）
- [ ] 引入线程池处理业务逻辑
- [ ] 定时器 Timer 支持
- [ ] 日志系统

---
*Created by [chenly](https://github.com/chenly)*
