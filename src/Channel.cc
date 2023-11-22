#include <sys/epoll.h>
#include "Channel.h"
#include "Epoll.h"

Channel::Channel(Epoll& _ep, int _fd){
    m_epoll=std::make_shared<Epoll>(_ep);
    m_fd=_fd;
}


int Channel::getFd() { return m_fd;}

uint32_t Channel::getEvents() { return m_events;}

uint32_t Channel::getRevents() { return m_revents;}

void Channel::setRevents(uint32_t ev) {
    m_revents=ev;
}

bool Channel::InEpoll() {return m_inEpoll;}

void Channel::enableReading() {
    m_events = EPOLLIN|EPOLLET;
    m_epoll->updateChannel(this);
}

void Channel::setInEpoll() {
    m_inEpoll= true;
}

void Channel::setCallBack(std::function<void()> _func) {
    m_callback=_func;
}