//
// Created by King on 2023/11/24.
//

#include <iostream>
#include <memory>
#include <utility>
#include "Acceptor.h"
#include "Epoll.h"
#include "Socket.h"
#include "Server.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Channel.h"

Acceptor::Acceptor(EventLoop::shared_ptr& _loop):m_loop(_loop)  {

    m_sock= new Socket();
    m_addr=new InetAddress("0.0.0.0",8080);
    m_sock->bind(m_addr);
    m_sock->listen();
    m_sock->setnonblocking();
    std::cout<<"[INFO] new acceptor has created!!!"<<std::endl;
    m_acceptChannel=new Channel(m_loop,m_sock->getFd());
    std::function<void()> cb=[this] { AcceptConnection(); };
    m_acceptChannel->setCallBack(cb);
    m_acceptChannel->enableReading();
}

Acceptor::~Acceptor() {
    delete m_sock;
    delete m_acceptChannel;
    delete m_addr;

}

void Acceptor::AcceptConnection() {

    InetAddress* client_addr= new InetAddress();

    Socket client_socket=Socket(m_sock->accept(client_addr));
    std::cout<<"[INFO] New Client From Ip:"<<inet_ntoa(client_addr->m_addr.sin_addr)<<" "
             <<"Port: "<< ntohs(client_addr->m_addr.sin_port)<<" "
             <<"Socket Fd: "<<client_socket.getFd()<<std::endl;

    client_socket.setnonblocking();
    newConnectionCallback(&client_socket);
    delete client_addr;
}

void Acceptor::SetNewConnectionCallback(std::function<void(Socket *)> _callback) {
    newConnectionCallback=std::move(_callback);
}