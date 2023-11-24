#include <sys/epoll.h>
#include <iostream>
#include <utility>
#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"
#include "error.h"

Channel::Channel(std::shared_ptr<EventLoop> loop, int fd):m_eloop(loop), m_fd(fd) {}
int Channel::getFd() const { return m_fd;}

uint32_t Channel::getEvents() const { return m_events;}

uint32_t Channel::getRevents() const { return m_revents;}

void Channel::setRevents(uint32_t ev) {
    m_revents=ev;
}

bool Channel::InEpoll() const {return m_inEpoll;}

void Channel::enableReading() {
    Debugf("enable Reading", __PRETTY_FUNCTION__);
    m_events = EPOLLIN|EPOLLET;
    m_eloop->updateChannel(this);
}

void Channel::setInEpoll() {
    m_inEpoll= true;
}

void Channel::setCallBack(std::function<void()> _func) {
    Debugf("set CallBack", __PRETTY_FUNCTION__);
    m_callback=std::move(_func);
}

void Channel::handleEvent() {
    m_callback();
}