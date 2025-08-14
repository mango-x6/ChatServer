#include "chatserver.hpp"
#include "chatservice.hpp"

using namespace std::placeholders;

// 初始化服务器
ChatServer::ChatServer(muduo::net::EventLoop *loop,
                       const muduo::net::InetAddress &listenAddr,
                       const std::string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 新用户连接回调
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // 断开连接
    if (!conn->connected())
    {
        ChatService::getInstance()->clientExceptionClose(conn);
        conn->shutdown();
    }
}

// 读写事件相关信息回调
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp t)
{
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf);

    // 根据msgid调用消息处理回调
    auto msgHandler = ChatService::getInstance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn, js, t);
}