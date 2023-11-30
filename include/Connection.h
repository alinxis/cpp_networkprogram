//
// Created by King on 2023/11/30.
//

#ifndef CPPNET_CONNECTION_H
#define CPPNET_CONNECTION_H

#include <functional>
#include <memory>
class EventLoop;
class Socket;
class Channel;
class Connection
{
public:
    typedef std::unique_ptr<Connection> unique_ptr;
    typedef std::shared_ptr<Connection> shared_ptr;

private:
    std::shared_ptr<EventLoop> m_loop;
    Socket *m_sock;
    std::unique_ptr<Channel> m_channel;
    std::function<void(Socket*)> deleteConnectionCallback;
public:


    Connection(std::shared_ptr<EventLoop>& _loop, Socket *_sock);
    ~Connection()=default;//使用编译器默认提供的，由于使用了智能指针所以不需担心内存泄露

    void echo(int sockfd);
    void setDeleteConnectionCallback(std::function<void(Socket*)>);
};

#endif //CPPNET_CONNECTION_H
