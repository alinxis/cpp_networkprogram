//
// Created by King on 2023/11/14.
//

#ifndef CPPNET_INETADDRESS_H
#define CPPNET_INETADDRESS_H
#include <arpa/inet.h>
#include <cstring>

class InetAddress
{
public:
    struct sockaddr_in m_addr;
    socklen_t m_addr_len=socklen_t(sizeof (sockaddr_in));//c++ 17起才支持，支持低版本请使用初始化列表

    InetAddress(){ bzero(&m_addr,sizeof(struct sockaddr_in)) ;}
    InetAddress(const char* ip, uint16_t port);
    ~InetAddress()=default;//目前使用编译器生成的就行
    void clear(){ bzero(&m_addr,sizeof(struct sockaddr_in)); }//加入一个清空函数，方便复用
};

#endif //CPPNET_INETADDRESS_H
