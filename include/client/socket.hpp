#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdexcept>

class SocketConn
{
public:
    static SocketConn *getInstance();

    // 连接管理
    bool start();
    void closeConn();
    bool isConnected() const;

    // 配置方法（只能在未连接时调用）
    void setPort(uint16_t port);
    void setIp(const std::string &ip); // 使用const引用

    // 禁用复制和赋值
    SocketConn(const SocketConn &) = delete;
    SocketConn &operator=(const SocketConn &) = delete;

    // 新增网络通信函数
    bool sendData(const std::string &);

    ssize_t receiveData(std::string &);

    // 发送并等待回复的核心函数
    bool sendAndReceive(const std::string &, std::string &);

private:
    SocketConn();
    ~SocketConn();

    sockaddr_in _server;
    int _fd;
    bool isRunning;
};

#endif