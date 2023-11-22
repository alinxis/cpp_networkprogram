#include <cstring>
#include <csignal>
#include <cstdio>
#include <cerrno>
#include <iostream>

#include "Server.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "error.h"
#define BUFFER_SIZE 1024

Server::Server(EventLoop *ep) {

    m_loop=EventLoop::ptr(ep);
    DEBUG("Init Socket",__PRETTY_FUNCTION__ );
    //初始化socket
    Socket* server_socket =new Socket();
    InetAddress server_addr =InetAddress("0.0.0.0",8080);
    server_socket->bind(server_addr);
    server_socket->listen();
    server_socket->setnonblocking();
    //将sccket 事件加入到epoll示例中用于处理
    Channel* serverChan = new Channel(m_loop,server_socket->getFd());
    std::function<void()> handler=std::bind(&Server::newConnection,this,server_socket);
    serverChan->setCallBack(handler);
    DEBUG("Server channel enableReading",__PRETTY_FUNCTION__ );
    serverChan->enableReading();
}


void Server::handleReadEvent(int fd){
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

void Server::newConnection(Socket *serv_sock) {

    InetAddress client_addr=InetAddress();
    Socket client_socket=Socket(serv_sock->accept(client_addr));

    printf("new client fd %d From IP: %s Port: %d\n",
           client_socket.getFd(), inet_ntoa(client_addr.m_addr.sin_addr),
           ntohs(client_addr.m_addr.sin_port));

    client_socket.setnonblocking();
    Channel *client_chan=new Channel(m_loop,client_socket.getFd());
    std::function<void()> func=std::bind(&Server::handleReadEvent,this,client_socket.getFd());
    client_chan->setCallBack(func);
    DEBUG("Client channel enableReading",__PRETTY_FUNCTION__ );
    client_chan->enableReading();

}