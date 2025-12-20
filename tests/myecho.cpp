/*
* @author: chenly
* @time: 2025-12-18 
*/


#include <iostream>
#include "../include/Socket.h"
#include "../include/InetAddress.h"

int main() {
    Socket server_socket;
    InetAddress addr("127.0.0.1", 8080);
    server_socket.Bind(addr);
    server_socket.Listen();

    while(true) {
        InetAddress client_addr;
        int client_fd = server_socket.Accept(client_addr);

        std::cout << "New Client Connection" << std::endl;

        //调试server端用
        char buffer[1024];
        while(true) {
            ssize_t read_byte = read(client_fd, buffer, sizeof(buffer) - 1);
            if (read_byte < 0) {
                perror("read failed");
                close(client_fd);
                break;
            }
            if (read_byte == 0) {
                std::cout << "Client Disconnected" << std::endl;
                close(client_fd);
                break;
            }
            buffer[read_byte] = '\0';
            std::cout << "Recv: " << buffer << std::endl;
            write(client_fd, buffer, read_byte);
        }
        
    }
    close(server_socket.GetFd());   
    return 0;
}