#include <iostream>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <cstring>
#include <csignal>
#include <fcntl.h>

#include "EventLoop.h"
#include "Server.h"



//处理echo事件
//void handleEvent(int fd){
//    char buffer[BUFFER_SIZE];
//    while(true) {    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
//        bzero(&buffer, sizeof(buffer));
//        ssize_t recv_len = read(fd, buffer, sizeof(buffer));
//        if (recv_len > 0) {
//            printf("[Msg] message from client fd %d: %s\n", fd, buffer);
//            write(fd, buffer, sizeof(buffer));
//            continue;
//        }
//        if (recv_len == -1 && errno == EINTR) {  //客户端正常中断、继续读取
//            printf("[info] continue reading");
//            continue;
//        }
//        if (recv_len == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {//非阻塞IO，这个条件表示数据全部读取完毕
//            printf("[info] finish reading once, errno: %d\n", errno);
//            break;
//        }
//        if (recv_len == 0) {  //EOF，客户端断开连接
//            printf("EOF, client fd %d disconnected\n", fd);
//            close(fd);   //关闭socket会自动将文件描述符从epoll树上移除
//            break;
//        }
//    }
//}

int main() {
   EventLoop::shared_ptr loop=std::make_shared<EventLoop>();
   Server *server=new Server(loop);
   loop->Loop();

   return 0;
}
