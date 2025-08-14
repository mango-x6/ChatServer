#include "socket.hpp"
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

SocketConn *SocketConn::getInstance()
{
    static SocketConn instance;
    return &instance;
}

SocketConn::SocketConn() : _fd(-1), isRunning(false)
{
    memset(&_server, 0, sizeof(sockaddr_in));
    _server.sin_family = AF_INET;
}

SocketConn::~SocketConn()
{
    if (_fd != -1)
    {
        close(_fd);
    }
}

bool SocketConn::start()
{
    if (isRunning)
    {
        std::cerr << "Connection already established" << std::endl;
        return false;
    }

    // 检查端口和IP是否已设置
    if (_server.sin_port == 0 || _server.sin_addr.s_addr == INADDR_NONE)
    {
        std::cerr << "IP or port not configured" << std::endl;
        return false;
    }

    // 创建socket
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd == -1)
    {
        perror("socket creation failed");
        return false;
    }

    // 连接服务器
    if (connect(_fd, (sockaddr *)&_server, sizeof(sockaddr_in)) == -1)
    {
        perror("connect failed");
        close(_fd); // 清理资源
        _fd = -1;
        return false;
    }

    isRunning = true;
    return true;
}

void SocketConn::closeConn()
{
    if (_fd != -1)
    {
        close(_fd);
        _fd = -1;
    }
    isRunning = false;
}

bool SocketConn::isConnected() const
{
    return isRunning;
}

void SocketConn::setPort(uint16_t port)
{
    if (isRunning)
    {
        throw std::logic_error("Cannot change port while connected");
    }
    _server.sin_port = htons(port);
}

void SocketConn::setIp(const std::string &ip)
{
    if (isRunning)
    {
        throw std::logic_error("Cannot change IP while connected");
    }

    if (inet_pton(AF_INET, ip.c_str(), &_server.sin_addr) <= 0)
    {
        throw std::invalid_argument("Invalid IP address format");
    }
}

bool SocketConn::sendData(const std::string &data)
{
    if (!isRunning)
    {
        std::cerr << "Not connected to server" << std::endl;
        return false;
    }
    size_t totalSent = 0, length = data.size();
    while (totalSent < length)
    {
        ssize_t sent = send(_fd, data.data() + totalSent, length - totalSent, 0);
        if (sent < 0)
        {
            if (errno == EINTR)
                continue; // 系统调用被中断
            perror("send failed");
            return false;
        }
        totalSent += sent;
    }
    return true;
}

ssize_t SocketConn::receiveData(std::string &data)
{
    if (!isRunning)
    {
        std::cerr << "Not connected to server" << std::endl;
        return -1;
    }
    char buffer[1024] = {0};
    ssize_t received = recv(_fd, buffer, 1024, 0);
    if (received < 0)
    {
        perror("receive failed");
    }
    data = buffer;
    return received; // >0: 实际长度, 0: 对方关闭, -1: 错误
}

bool SocketConn::sendAndReceive(const std::string &send, std::string &recv)
{
    // 1. 发送数据
    if (!sendData(send))
    {
        return false;
    }
    // 2. 等待并接收响应
    ssize_t received = receiveData(recv);
    if (received < 0)
        return false; // 错误
    return true;
}