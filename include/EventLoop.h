//
// Created by King on 2023/11/22.
//

#ifndef CPPNET_EVENTLOOP_H
#define CPPNET_EVENTLOOP_H
#include <memory>

class Epoll;
class Channel;

class EventLoop {
private:
    Epoll* m_epoll; //在这个事件循环中，epoll没有被外部访问，也并非为外部传参，生命周期在类内很安全，不需要智能指针管理
    bool quit;

public:
    typedef std::shared_ptr<EventLoop> ptr;
    EventLoop();
    ~EventLoop();

    void Loop();
    void updateChannel(Channel* );
};


#endif //CPPNET_EVENTLOOP_H
