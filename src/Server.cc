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
#include "Connection.h"
Server::Server(EventLoop::shared_ptr& ep ):m_loop(ep) {

    m_acceptor=std::make_unique<Acceptor>(ep);
    Acceptor::CallBackFunc func=std::bind(&Server::newConnection, this,std::placeholders::_1);
    m_acceptor->SetNewConnectionCallback(func);
}

//Server::~Server() {
//
//}


void Server::handleReadEvent(int fd){

}

void Server::newConnection(Socket *sock) {
    Connection* conn=new Connection(m_loop,sock);
    m_connections[sock->getFd()]=conn;//添加conn
    std::function<void(Socket*)> func= std::bind(&Server::deleteConnection,this,std::placeholders::_1);
    conn->setDeleteConnectionCallback(func);
}

void Server::deleteConnection(Socket * sock){
    Connection *conn = m_connections[sock->getFd()];
    m_connections.erase(sock->getFd());
    delete conn;
}