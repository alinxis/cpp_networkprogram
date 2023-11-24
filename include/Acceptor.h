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
    /* 对于智能指针应用是应该是克制的，应该根据其生命周期来使用；
     * 如果其生命周期是明确的，访问情况是明确可控的,就应该不使用智能指针；
     * 如果不知道该不该用智能指针，那么share_ptr 是一个好的选择，这仅在类的属性内有效，函数参数不可以随便设置为智能指针；
     * 但是请注意，智能指针也具有开销,一般来说
     * unique 代表独占资源，一个类控制着这个资源的生命周期时就应该使用
     * share 代表多个类对一个资源有访问需求，当所有类都不需要时才能进行销毁
     * raw  裸指针，仅仅代表对资源有访问权，并不能管理其声明周期，这会出现访问失效的情况
     */

    std::shared_ptr<EventLoop> m_loop;
    Socket *m_sock;
    InetAddress* m_addr;
    Channel *m_acceptChannel;
public:
    typedef std::function<void(Socket*)> CallBackFunc;
    typedef std::shared_ptr<Acceptor> shared_ptr;
    typedef std::unique_ptr<Acceptor> unique_ptr;

    std::function<void(Socket*)> newConnectionCallback;

    Acceptor(std::shared_ptr<EventLoop>&);
    ~Acceptor();
    void AcceptConnection();
    void SetNewConnectionCallback(std::function<void(Socket*)>);
};
#endif //CPPNET_ACCEPTOR_H
