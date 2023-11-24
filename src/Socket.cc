#include <fcntl.h>
#include <sys/socket.h>
#include <iostream>

#include "InetAddress.h"
#include "Socket.h"
#include "error.h"

Socket::Socket() {
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    errorf(m_fd == -1, "[ERROR] socket create error");
}
Socket::Socket(int _fd) : m_fd(_fd){
    errorf(m_fd == -1, "[ERROR] socket create error");
}


void Socket::setnonblocking() {
    // 获取当前文件描述符的标志
    int flags = fcntl(m_fd, F_GETFL, -1);
    errorf(flags == -2, "[ERROR] fcntl get error");
    flags |= O_NONBLOCK;
    // 设置新的文件描述符标志
    errorf(fcntl(m_fd, F_SETFL, flags) == -2, "[ERROR] fcntl set error");
}

void Socket::bind(InetAddress::share_ptr addr){
    int errorCode=::bind(m_fd, reinterpret_cast<sockaddr*>(&addr->m_addr) , addr->m_addr_len);
    errorf( errorCode== -1, "socket bind error");
}

void Socket::listen(){
    std::cout<<"[INFO] Start Listen"<<std::endl;
    errorf(::listen(m_fd, SOMAXCONN) == -1, "socket listen error");
}


int Socket::accept(InetAddress::unique_ptr addr){
    int clnt_sockfd = ::accept(m_fd, reinterpret_cast<sockaddr*>(&addr->m_addr), &addr->m_addr_len);
    errorf(clnt_sockfd == -1, "socket accept error");
    return clnt_sockfd;
}

int Socket::getFd(){
    return m_fd;
}