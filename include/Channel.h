//
// Created by King on 2023/11/21.
//


#ifndef CPPNET_CHANNEL_H
#define CPPNET_CHANNEL_H

#include <cstdint>
#include <memory>

class Epoll;

class Channel {
private:
    std::shared_ptr<Epoll> m_epoll;
    int m_fd;                       //socketfd
    uint32_t m_events = 0;          //注册时要监听的事件
    uint32_t m_revents = 0;         //Epoll 返回时发生的事件
    bool m_inEpoll = false;         //采用默认值

public:
    typedef std::shared_ptr<Channel> ptr;

    Channel(Epoll &_ep, int _fd);
    ~Channel() = default;

    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    void setRevents(uint32_t);
    bool InEpoll();
    void setInEpoll();

};


#endif //CPPNET_CHANNEL_H
