#include <sys/epoll.h>
#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"
#include "error.h"

Channel::Channel(EventLoop* _eloop, int _fd){
    m_eloop=EventLoop::ptr(_eloop);
    m_fd=_fd;
}


int Channel::getFd() const { return m_fd;}

uint32_t Channel::getEvents() const { return m_events;}

uint32_t Channel::getRevents() const { return m_revents;}

void Channel::setRevents(uint32_t ev) {
    m_revents=ev;
}

bool Channel::InEpoll() const {return m_inEpoll;}

void Channel::enableReading() {
    DEBUG("enable Reading",__PRETTY_FUNCTION__ );
    m_events = EPOLLIN|EPOLLET;
    m_eloop->updateChannel(this);
}

void Channel::setInEpoll() {
    m_inEpoll= true;
}

void Channel::setCallBack(std::function<void()> _func) {
    DEBUG("set CallBack",__PRETTY_FUNCTION__ );
    m_callback=_func;
}

void Channel::handleEvent() {
    m_callback();
}