# MyTinyWeb

一个基于 **Reactor 模式** 的轻量级 C++ 高性能网络服务器雏形。本项目旨在从底层 Socket 编程开始，逐步构建一个支持高并发、具备现代 C++ 特性的 Web 服务器。

## 🚀 当前版本功能 (V1.0 - Echo 阶段)
- **核心组件封装**：封装了 `Socket`、`InetAddress`、`Epoll` 和 `Channel`。
- **Reactor 模式雏形**：使用事件分发机制，解耦基础网络 IO 与业务逻辑。
- **高性能 IO**：基于 Linux Epoll 边缘触发 (ET) 模式。
- **自定义缓冲区 (Buffer)**：实现支持动态扩容、自动数据挪动的应用层缓冲区，解决粘包与非阻塞 IO 读写问题。
- **现代 C++ 风格**：广泛使用智能指针 (RAII)、Lambda 表达式、`std::function` 以及 C++14 特性。

## 🏗️ 架构设计
- **EventLoop**：运行在主线程的事件循环，负责监听并分发 IO 事件。
- **Channel**：封装 Fd 及其关心的事件，并维护相关的回调函数。
- **Buffer**：双游标设计的输入输出缓冲区，优化内存拷贝并支持自动扩容。

## 🛠️ 如何编译与运行
### 依赖环境
- Linux 系统 (由于使用了 Epoll)
- CMake (3.10+)
- 支持 C++17 的编译器 (如 GCC 7+)

### 编译步骤
```bash
mkdir build
cd build
cmake ..
make
```

### 运行 Echo 服务测试
```bash
./tests/server
```

## 📅 下一步计划
- [ ] 引入 `TcpConnection` 进一步完善封装。
- [ ] 实现 Multi-Reactor 模式（主从从 Reactor 模型）。
- [ ] 引入线程池处理业务逻辑。
- [ ] 支持 HTTP 协议解析。

---
*Created by [chenly](https://github.com/chenly)*
