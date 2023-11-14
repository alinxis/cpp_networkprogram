//
// Created by King on 2023/11/14.
//

#include <cstring>
#include <csignal>
#include "Epoll.h"
#include "error.h"


Epoll::Epoll()  {
    m_epfd=epoll_create1(0);
    errorf(m_epfd==-1,"[ERROR] epoll create error !");
    m_events=new epoll_event[MAX_EVENTS];
    bzero(m_events, sizeof(epoll_event) * MAX_EVENTS);
}
Epoll::~Epoll(){
    if(m_epfd!=-1) close(m_epfd);
    delete[] m_events;
}
void Epoll::AddFd(int fd, uint32_t op){
    epoll_event ev;
    bzero(&ev, sizeof(epoll_event));
    ev.data.fd = fd;
    ev.events = op;
    errorf(epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "[ERROR] epoll add event error");
}
std::vector<epoll_event> Epoll::Poll(int timeout) {
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(m_epfd,m_events,MAX_EVENTS,timeout);
    for (int i=0 ;i<nfds;i++) {
        activeEvents.push_back(m_events[i]);
    }
    return  activeEvents;
    //return std::move(activeEvents);
    /*
     * 使用std::move并不是必需的，因为编译器会自动执行返回值优化（RVO），以避免不必要的拷贝。
     * 实际上，在某些情况下，显式地使用std::move可能会禁用RVO，从而导致性能下降。
     * 因此，在这种情况下，最好遵循“不要过早优化”的原则，让编译器自动处理优化。
     */
}