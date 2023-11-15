//
// Created by King on 2023/11/14.
//

#ifndef CPPNET_SOCKET_H
#define CPPNET_SOCKET_H
class InetAddress;
class Socket
{
private:
    int m_fd;
public:
    Socket();
    Socket(int);
    ~Socket()=default;//使用提供的默认析构函数就可以

    void bind(InetAddress&);
    void listen();
    void setnonblocking();

    int accept(InetAddress&);

    int getFd();
};
#endif //CPPNET_SOCKET_H
