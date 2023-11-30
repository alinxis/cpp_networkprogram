#include <cstring>
#include <cerrno>
#include <cstdio>
#include <csignal>
#include <utility>
#include "Channel.h"
#include "Connection.h"
#include "Socket.h"

#define BUFFER_SIZE 1024

Connection::Connection(std::shared_ptr<EventLoop> &_loop, Socket *_sock):m_loop(_loop),m_sock(_sock) {
    m_channel=std::make_unique<Channel>(_loop,_sock->getFd());
    std::function<void()> callbackfunc=std::bind(&Connection::echo, this, _sock->getFd());
    m_channel->setCallBack(callbackfunc);
    m_channel->enableReading();
}

void Connection::echo(int sockfd) {
    char buffer[BUFFER_SIZE];
    while(true) {    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buffer, sizeof(buffer));
        ssize_t recv_len = read(sockfd, buffer, sizeof(buffer));
        if (recv_len > 0) {
            printf("[Msg] message from client fd %d: %s\n", sockfd, buffer);
            write(sockfd, buffer, sizeof(buffer));
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
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> func) {
    deleteConnectionCallback=std::move(func);
}