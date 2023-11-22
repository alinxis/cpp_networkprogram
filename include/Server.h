//
// Created by King on 2023/11/22.
//

#ifndef CPPNET_SERVER_H
#define CPPNET_SERVER_H

#include <memory>

class EventLoop;
class Socket;
class Server
{
private:
     std::shared_ptr<EventLoop> m_loop;
public:
    Server(EventLoop*);
    ~Server()=default;

    void handleReadEvent(int);
    void newConnection(Socket *serv_sock);
};
#endif //CPPNET_SERVER_H
