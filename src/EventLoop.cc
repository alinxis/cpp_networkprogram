#include <vector>

#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "error.h"
EventLoop::EventLoop() {
    m_epoll=new Epoll();

}

EventLoop::~EventLoop(){
    delete m_epoll;
}

void EventLoop::Loop() {
    DEBUG("Loop start",__PRETTY_FUNCTION__ );
    while (!quit){
        std::vector<Channel::ptr> activeChannel=m_epoll->poll();
        for (auto chan: activeChannel){
                chan->handleEvent();
        }

    }
}

void EventLoop::updateChannel(Channel* chan) {
    m_epoll->updateChannel(chan);
}