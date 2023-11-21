#include <iostream>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <cstring>
#include <csignal>
#include <fcntl.h>

#include "Channel.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Epoll.h"
#include "error.h"

#define BUFFER_SIZE 1024

//处理echo事件
void handleEvent(int fd){
    char buffer[BUFFER_SIZE];
    while(true) {    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buffer, sizeof(buffer));
        ssize_t recv_len = read(fd, buffer, sizeof(buffer));
        if (recv_len > 0) {
            printf("[Msg] message from client fd %d: %s\n", fd, buffer);
            write(fd, buffer, sizeof(buffer));
            continue;
        }
        if (recv_len == -1 && errno == EINTR) {  //客户端正常中断、继续读取
            printf("[info] continue reading");
            continue;
        }
        if (recv_len == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {//非阻塞IO，这个条件表示数据全部读取完毕
            printf("[info] finish reading once, errno: %d\n", errno);
            break;
        }
        if (recv_len == 0) {  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", fd);
            close(fd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}

int main() {
    //创建套接字描述符
    Socket serv_socket=Socket();
    InetAddress addr=InetAddress("0.0.0.0",8080);
    serv_socket.bind(addr);
    serv_socket.listen();

    //创建epoll 实例
    Epoll epoll;
    Channel* server=new Channel(epoll,serv_socket.getFd());
    server->enableReading();

    while (true){
        std::vector<Channel::ptr> activeChannels =epoll.poll();
        for (auto chan: activeChannels) {
            //如果为服务的scoket就接受连接
            InetAddress client_addr=InetAddress();
            int chfd=chan->getFd();
            if(chfd==serv_socket.getFd()){
                client_addr.clear();
                int client_fd=serv_socket.accept(client_addr); //接受连接并保存连接信息
                Socket client_socket=Socket(client_fd);//创建连接的客户端
                client_socket.setnonblocking();

                printf("new client fd %d From IP: %s Port: %d\n",
                       client_socket.getFd(), inet_ntoa(client_addr.m_addr.sin_addr),
                       ntohs(client_addr.m_addr.sin_port));

                Channel* client=new Channel(epoll,client_socket.getFd());
                client->enableReading();//这里其实并不好，虽然代码少写一行，但是意图变得不连贯了

            } else if(chan->getRevents()&EPOLLIN){//如果是客户端的话
                handleEvent(chan->getFd());
            } else{
                std::cout<<"[INFO] something happen"<<std::endl;
            }
        }

    }

    return 0;
}
