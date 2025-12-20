#pragma once
//using namespace std;  //一般在.h里面不用这个
//template <typename T>  //socket不需要用模板类
#include <unistd.h>

class InetAddress;

class Socket 
{
public:
    Socket();
    explicit Socket(int fd) : _fd(fd) {};
    ~Socket() { close(_fd); }

public:
    int GetFd() const { return _fd; }
    void Bind(const InetAddress& addr);
    void Listen();
    int Accept(InetAddress& addr);
    void SetNonBlocking();

private:
    int _fd;
};