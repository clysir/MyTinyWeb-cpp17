/*
@ author: chenly
@ time: 2025-12-19 13:03:25
*/


#include "../include/Epoll.h"
#include "../include/Socket.h"
#include "../include/InetAddress.h"
#include <iostream>

int main() {
    //1. setup
    Socket ser_socket;
    InetAddress addr("127.0.0.1", 8080);
    ser_socket.Bind(addr);
    ser_socket.Listen();

    Epoll ep;
    ep.addFd(ser_socket.GetFd(), EPOLLIN); //将服务器放进epoll监听

    //2. Loop
    while(1) {
        std::vector<epoll_event> events = ep.poll();

        for(auto event : events) {
            int fd = event.data.fd;

            if (fd == ser_socket.GetFd()) {  //如果是服务器 说明有新连接
                InetAddress client_addr;
                int client_fd = ser_socket.Accept(client_addr);
                std::cout << "New Client fd: " << client_fd << std::endl;  
                ep.addFd(client_fd, EPOLLIN); //将新连接放进epoll监听
            } 
            else {
                char buffer[1024];

                ssize_t read_byte = read(fd,buffer,sizeof(buffer) - 1);

                if(read_byte > 0) {
                    buffer[read_byte] = '\0';
                    std::cout << "Recv from fd: [" << fd << "]: " << buffer << std::endl;
                    write(fd,buffer,read_byte); 
                }
                else if(read_byte == 0) {
                    std::cout << "Client Disconnected fd: [" << fd << "]" << std::endl;
                    close(fd);  //关闭连接 这个时候 epoll会自动移除
                    //ep.delFd(fd); //显示关闭 
                }
                else {
                    perror("read failed");
                    close(fd);
                }
                
            }
        }
    }
    return 0;
}