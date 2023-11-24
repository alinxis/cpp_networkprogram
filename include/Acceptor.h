//
// Created by King on 2023/11/24.
//

#ifndef CPPNET_ACCEPTOR_H
#define CPPNET_ACCEPTOR_H

#include <functional>
#include <memory>

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor{
private:
    /* 对于智能指针应用是克制的，应该根据其生命周期来使用
     * 如果其生命周期是明确的，就可以不使用智能指针
     * 如果不知道该不该用智能指针，那么share_ptr 是一个好的选择
     */

    std::shared_ptr<EventLoop> m_loop;
    Socket *m_sock;
    std::shared_ptr<InetAddress> m_addr;
    Channel *m_acceptChannel;
public:
    typedef std::function<void(Socket*)> CallBackFunc;
    typedef std::shared_ptr<Acceptor> ptr;

    std::function<void(Socket*)> newConnectionCallback;

    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void AcceptConnection();
    void SetNewConnectionCallback(std::function<void(Socket*)>);
};
#endif //CPPNET_ACCEPTOR_H
