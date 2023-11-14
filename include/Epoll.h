//
// Created by King on 2023/11/14.
//

#ifndef CPPNET_EPOLL_H
#define CPPNET_EPOLL_H

#include <sys/epoll.h>
#include <cstdint>
#include <vector>
class Epoll {
private:
    int m_epfd;
    epoll_event *m_events;

public:
    static constexpr int MAX_EVENTS=1000;
    Epoll();
    ~Epoll();

    void AddFd(int fd,uint32_t option);
    std::vector<epoll_event> Poll(int timeout = -1);
};


#endif //CPPNET_EPOLL_H
