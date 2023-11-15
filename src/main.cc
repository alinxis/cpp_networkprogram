#include <iostream>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <cstring>
#include <csignal>
#include <fcntl.h>

#include "Socket.h"
#include "InetAddress.h"
#include "Epoll.h"
#include "error.h"

#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

//设置非阻塞状态
void setnonblocking(int fd) {

    // 获取当前文件描述符的标志
    int  flags = fcntl(fd, F_GETFL, -1);
    if (flags == -2) {
        perror("fcntl");
        return;
    }

    // 将O_NONBLOCK标志添加到文件描述符的标志中
    flags |= O_NONBLOCK;
    // 设置新的文件描述符标志
    if (fcntl(fd, F_SETFL, flags) == -2) {
        perror("fcntl");
        return;
    }
}

//处理echo事件
void handleEvent(int fd){
    char buffer[BUFFER_SIZE];
    while(true) {    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buffer, sizeof(buffer));
        ssize_t recv_len = read(fd, buffer, sizeof(buffer));
        if (recv_len > 0) {
            printf("[Msg] message from client fd %d: %s\n", fd, buffer);
            write(fd, buffer, sizeof(buffer));
            continue;
        }
        if (recv_len == -1 && errno == EINTR) {  //客户端正常中断、继续读取
            printf("[info] continue reading");
            continue;
        }
        if (recv_len == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {//非阻塞IO，这个条件表示数据全部读取完毕
            printf("[info] finish reading once, errno: %d\n", errno);
            break;
        }
        if (recv_len == 0) {  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", fd);
            close(fd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}

int main() {
    //创建套接字描述符
    Socket serv_socket=Socket();
    InetAddress addr("0.0.0.0",8080);
    serv_socket.bind(addr);
    serv_socket.listen();

    //创建epoll 实例
    Epoll epoll;
    epoll.addFd(serv_socket.getFd(), EPOLLIN | EPOLLET); //设置epoll 为et模式

    while (true){
        std::vector<epoll_event> events =epoll.poll();
        for (auto& event: events) {
            //如果为服务的scoket就接受连接
            InetAddress client_addr=InetAddress();
            if(event.data.fd==serv_socket.getFd()){
                client_addr.clear();
                int client_fd=serv_socket.accept(client_addr); //接受连接并保存连接信息
                Socket client_socket=Socket();//创建连接的服务端
                client_socket.setnonblocking();

                printf("new client fd %d From IP: %s Port: %d\n",
                       client_socket.getFd(), inet_ntoa(client_addr.m_addr.sin_addr),
                       ntohs(client_addr.m_addr.sin_port));
                epoll.addFd(client_fd, EPOLLIN | EPOLLET); //添加事件

            }
            if(event.events&EPOLLIN){
                handleEvent(event.data.fd);
            } else{
                std::cout<<"[INFO] something happen"<<std::endl;
            }
        }

    }

    return 0;
}
