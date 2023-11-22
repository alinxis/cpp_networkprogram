//
// Created by King on 2023/11/14.
//

#ifndef CPPNET_EPOLL_H
#define CPPNET_EPOLL_H

#include <sys/epoll.h>
#include <cstdint>
#include <vector>

class Channel;
class Epoll {
private:
    int m_epfd;
    epoll_event *m_events;

public:
    typedef std::shared_ptr<Epoll> share_ptr;
    static constexpr int MAX_EVENTS=1000;
    Epoll();
    ~Epoll();

    void addFd(int fd,uint32_t option);
    std::vector<std::shared_ptr<Channel>> poll(int timeout = -1);
    void updateChannel(Channel* chan);
};


#endif //CPPNET_EPOLL_H
