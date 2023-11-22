#include <vector>

#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

EventLoop::EventLoop() {
    m_epoll=new Epoll();

}

EventLoop::~EventLoop(){
    delete m_epoll;
}

void EventLoop::Loop() {
    while (!quit){
        std::vector<Channel::ptr> activeChannel;
        for (auto chan: activeChannel){
                chan->handleEvent();
        }

    }
}

void EventLoop::updateChannel(Channel* chan) {
    m_epoll->updateChannel(chan);
}