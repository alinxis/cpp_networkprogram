#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <cstring>
#include <csignal>
#include <fcntl.h>

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
    int error_code=0;
    int sock_fd= socket(AF_INET,SOCK_STREAM,0);
    errorf(sock_fd==-1,"socket create error");
    //设置地址相关信息
    struct sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr) );
    serv_addr.sin_addr.s_addr =inet_addr("0.0.0.0");
    serv_addr.sin_port= htons(8888);
    //绑定描述符
    error_code=bind(sock_fd, reinterpret_cast<const sockaddr *>(&serv_addr), sizeof(serv_addr));
    errorf(error_code==-1,"bind error");
    //监听
    printf("[info] start listening ... ...\n");
    error_code=listen(sock_fd,SOMAXCONN);
    errorf(error_code==-1,"listen error");

    //创建epoll实例
    int epfd= epoll_create1(0);
    //注册事件
    epoll_event ev;
    ev.events=EPOLLIN;
    ev.data.fd=sock_fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sock_fd,&ev);

    epoll_event events[MAX_EVENTS];//可触发事件的缓冲区
    //监听epoll事件
    while (true){
        int nfds= epoll_wait(epfd,events,MAX_EVENTS,-1);//有nfds个fd发生事件
        //处理发生的事件
        for(int i = 0;i<nfds;i++){
            //处理连接事件
            if(events[i].data.fd==sock_fd){//发生事件的fd是服务器socket fd，表示有新客户端连接
                struct sockaddr_in client_addr;
                bzero(&client_addr,sizeof(client_addr));
                socklen_t clint_addr_len= sizeof(client_addr);
                int client_sockfd= accept(sock_fd, reinterpret_cast<sockaddr *>(&client_addr), &clint_addr_len);
                printf("[info] new client fd %d! IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr)
                        , ntohs(client_addr.sin_port));
                setnonblocking(client_sockfd);
                //加入epoll 监听
                ev.events=EPOLLIN|EPOLLET;
                ev.data.fd=client_sockfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,client_sockfd,&ev);
                continue;
            }

            if(events[i].events&EPOLLIN){//如果是输入事件
                handleEvent(events[i].data.fd);
            }
        }
    }
    return 0;
}
