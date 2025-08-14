#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    // 初始化服务器
    ChatServer(::EventLoop *loop,
               const InetAddress &listenAddr,
               const std::string &nameArg);

    // 启动服务
    void start();

private:
    // 新用户连接回调
    void onConnection(const TcpConnectionPtr &);
    // 读写事件相关信息回调
    void onMessage(const TcpConnectionPtr &, Buffer *, Timestamp);

    ::TcpServer _server;
    ::EventLoop *_loop;
};

#endif