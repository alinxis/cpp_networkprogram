# cpp_networkprogram
一个从零开始学习cpp网络编程的旅程
预计会完成基本的学习，后期可能会参照muduo继续学习，采用c++17实现（也许）

#### day01
- 初步实现epoll模型的echo服务器

TODO 实现代码的重构

#### day02
- 完成代码文件结构的初步重构
- 重构Epoll类
- 重构Inetaddress类
- 重构完成 socket类
- 重构服务端代码

原理请阅读linux高性能服务器编程一书

TODO 继续重构代码和学习原理

#### day03

- 完成对event的封装，将event初步封装到channel类中，这还很原始，不可以使用
- 重写服务端代码


因为身体原因，拖延了很久 commit时间在2023/11/21日的就是day03的修改的代码，前几天的就是之前的代码

#### day04

- 完成对Reactor模式的学习 资料的话参见 《linux高性能服务器编程》
- 完成了对Reactor模式的初步实现——这还很原始，不可以使用
- 重写服务端代码
- 加入了一点 DEBUG 代码

TODO: 未来会加入更完整的日志和debug方法，这个功能预计会实现的很晚，毕竟目前学习的重点不是这个

commit时间在2023/11/22日的就是day04的修改的代码

#### day05

- 完成加入acceptor 初步实现了Reactor,这仍然很原始，但是可用了
- 再次对智能指针的使用进行了修改，目前来说，还可能有不妥的地方

关于对智能指针的说明
* 对于智能指针应用是应该是克制的，应该根据其生命周期来使用；
* 如果其生命周期是明确的，访问情况是明确可控的,就应该不使用智能指针；
* 如果不知道该不该用智能指针，那么share_ptr 也许是一个好的选择，请注意函数参数不可以随便设置为智能指针，对于性能严格要求的地方share_ptr的使用也不应该随便；

但是请注意，智能指针也具有开销,一般来说
* unique 代表独占资源，只有一个类可以控制着这个资源的生命周期时就应该使用
* share 代表多个类对一个资源有访问需求，当所有类都不需要时才能进行销毁
* raw  裸指针，仅仅代表对资源有访问权，并不能管理其生命周期，这会出现访问失效的情况

实际上，在Acceptor 类中,原始指针都可以替换为unique_ptr,但是这些裸指针不存在将资源交给外部控制的情况
因此是可控行为，可以不使用unique_ptr
```c++
    class Acceptor{
    private:
        std::shared_ptr<EventLoop> m_loop;
        Socket *m_sock;
        InetAddress* m_addr;
        Channel *m_acceptChannel;
```

#### day06

把tcp也变为一个连接，对网络框架进一步的抽象，这个没什么变动很多，在提交里就能清晰的看到

#### day07-08

实现一个模板缓冲区，这个缓冲区的说明会放在一个explaination文件夹内（没写完），描述了我的实现过程以及思路，这个对于我这个小菜鸡来说，
实在是一个长足的进步，果然抄代码不如自己创造。