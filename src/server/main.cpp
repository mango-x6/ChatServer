#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>

using namespace std;
using namespace muduo;
using namespace muduo::net;

void resetHandler(int)
{
    ChatService::getInstance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
    // 参数校验（需要IP地址和端口号两个参数）
    if (argc < 3)
    {
        cerr << "command invalid!" << endl;
        exit(1);
    }

    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr(argv[1], atoi(argv[2]));
    ChatServer server(&loop, addr, "chatserver");

    server.start();
    loop.loop();

    return 0;
}
