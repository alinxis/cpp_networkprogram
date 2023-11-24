//
// Created by King on 2023/11/22.
//

#ifndef CPPNET_SERVER_H
#define CPPNET_SERVER_H

#include <memory>

class EventLoop;
class Socket;
class Acceptor;
class Server
{
private:
     std::shared_ptr<EventLoop> m_loop;
     Acceptor* m_acceptor;
public:

    explicit Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket *serv_sock);
};
#endif //CPPNET_SERVER_H
