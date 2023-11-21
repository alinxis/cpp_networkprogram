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

