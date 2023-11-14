//
// Created by King on 2023/11/14.
//

#include "InetAddress.h"

InetAddress::InetAddress(const char *ip, uint16_t port) {
    //初始化结构体
    bzero(&m_addr,sizeof(struct sockaddr_in));
    m_addr.sin_family=AF_INET;
    m_addr.sin_port= htons(port);
    m_addr.sin_addr.s_addr= inet_addr(ip);

}