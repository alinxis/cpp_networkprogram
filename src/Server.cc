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
#include "Acceptor.h"
#define BUFFER_SIZE 1024

Server::Server(EventLoop *ep):m_loop(ep) {
    m_acceptor=new Acceptor(ep);
    Acceptor::CallBackFunc func=std::bind(&Server::newConnection, this,std::placeholders::_1);
    m_acceptor->SetNewConnectionCallback(func);
}

Server::~Server() {
    delete m_acceptor;
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

    InetAddress::unique_ptr client_addr= std::make_unique<InetAddress>();

    std::cout<<"[INFO] New Client From Ip:"<<inet_ntoa(client_addr->m_addr.sin_addr)<<" "
            <<"Port: "<< ntohs(client_addr->m_addr.sin_port);

    Socket client_socket=Socket(serv_sock->accept(std::move(client_addr)));
    std::cout<<"Socket Fd: "<<client_socket.getFd()<<std::endl;


    client_socket.setnonblocking();
    Channel *client_chan=new Channel(m_loop,client_socket.getFd());
    std::function<void()> func=std::bind(&Server::handleReadEvent,this,client_socket.getFd());
    client_chan->setCallBack(func);
    Debugf("Client channel enableReading", __PRETTY_FUNCTION__);
    client_chan->enableReading();

}