cmake_minimum_required(VERSION 3.10)
project(Tests)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 开启调试和警告
set(CMAKE_CXX_FLAGS "-Wall -g")

# 1. 包含头文件路径 (../include)
include_directories(../include)

# 2. 定义核心库的源文件
# 以后增加了新的 cpp 也要往这里加
set(SRC_FILES
    ../src/Socket.cpp
    ../src/InetAddress.cpp
    ../src/Epoll.cpp
    ../src/Server.cpp
    ../src/EventLoop.cpp
    ../src/Channel.cpp
    ../src/TcpConnection.cpp
    ../src/Buffer.cpp
    ../src/Http/HttpContext.cpp
    ../src/Http/HttpRequest.cpp
    ../src/Http/HttpResponse.cpp
    ../src/Http/HttpServer.cpp
    ../src/Http/MimeType.cpp
)

# 3. 生成可执行文件
#add_executable(myecho myecho.cpp ${SRC_FILES})

#add_executable(epoll_server epoll_server.cpp ${SRC_FILES})

#add_executable(server server.cpp ${SRC_FILES})

#add_executable(http_server http_server.cpp ${SRC_FILES})
add_executable(http_web http_web.cpp ${SRC_FILES})
