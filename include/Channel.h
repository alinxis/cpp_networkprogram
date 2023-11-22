//
// Created by King on 2023/11/21.
//


#ifndef CPPNET_CHANNEL_H
#define CPPNET_CHANNEL_H

#include <cstdint>
#include <memory>
#include <functional>
#include <utility>

class EventLoop;

class Channel {
private:
    std::shared_ptr<EventLoop> m_eloop;
    int m_fd;                       //socketfd
    uint32_t m_events = 0;          //注册时要监听的事件
    uint32_t m_revents = 0;         //Epoll 返回时发生的事件
    bool m_inEpoll = false;         //采用默认值
    std::function<void()> m_callback;

public:
    typedef std::shared_ptr<Channel> ptr;

    Channel(EventLoop *_loop, int _fd);
    Channel(std::shared_ptr<EventLoop> loop,int fd):m_eloop(std::move(loop)), m_fd(fd) {}
    ~Channel() = default;

    int getFd() const;
    uint32_t getEvents() const;
    uint32_t getRevents() const;
    bool InEpoll() const;

    void setRevents(uint32_t);
    void setInEpoll();
    void setCallBack(std::function<void()>);

    void enableReading();
    void handleEvent();
};


#endif //CPPNET_CHANNEL_H
