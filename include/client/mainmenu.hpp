#ifndef MAINMENU_H
#define MAINMENU_H

#include <string>
#include <unordered_map>
#include <functional>
#include "socket.hpp"

using namespace std;

void help(SocketConn *, string);
void chat(SocketConn *, string);
void addfriend(SocketConn *, string);
void creategroup(SocketConn *, string);
void joingroup(SocketConn *, string);
void groupchat(SocketConn *, string);
void quit(SocketConn *, string);

class MainMenu
{
public:
    static MainMenu *getInstance();
    void setSocket(SocketConn *);
    function<void(string)> getHandler(string);

private:
    MainMenu() {}

    unordered_map<string, function<void(string)>> commandMap;
};

#endif