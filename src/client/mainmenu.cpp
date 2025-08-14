#include "mainmenu.hpp"
#include "socket.hpp"
#include <iostream>
#include "utils.hpp"

MainMenu *MainMenu::getInstance()
{
    static MainMenu menu;
    return &menu;
}
function<void(string)> MainMenu::getHandler(string s)
{
    unordered_map<string, function<void(string)>>::iterator it;
    if ((it = commandMap.find(s)) != commandMap.end())
    {
        return it->second;
    }
    return [](string = "")
    { cerr << "invalid input commend!" << endl; };
}
void MainMenu::setSocket(SocketConn *conn)
{
    commandMap.clear();
    commandMap.insert(make_pair("help", bind(help, nullptr, std::placeholders::_1)));
    commandMap.insert(make_pair("chat", bind(chat, conn, std::placeholders::_1)));
    commandMap.insert(make_pair("addfriend", bind(addfriend, conn, std::placeholders::_1)));
    commandMap.insert(make_pair("creategroup", bind(creategroup, conn, std::placeholders::_1)));
    commandMap.insert(make_pair("joingroup", bind(joingroup, conn, std::placeholders::_1)));
    commandMap.insert(make_pair("groupchat", bind(groupchat, conn, std::placeholders::_1)));
    commandMap.insert(make_pair("quit", bind(quit, conn, std::placeholders::_1)));
}
void help(SocketConn *conn = nullptr, string = "")
{
    unordered_map<string, string> helpMap{
        {"help", "help"},
        {"chat", "chat:toid:msg"},
        {"addfriend", "addfriend:friendid"},
        {"creategroup", "creategroup:groupname:groupdesc"},
        {"joingroup", "joingroup:groupid"},
        {"groupchat", "groupchat:groupid:msg"},
        {"quit", "quit"},
    };
    for (auto it : helpMap)
    {
        cout << it.first << "    " << it.second << endl;
    }
}
void chat(SocketConn *conn, string input)
{
    int idx;
    if ((idx = input.find(":")) == -1)
    {
        cerr << "invalid input commend!" << endl;
        return;
    }
    json js;
    js["msgid"] = EnMsgType::CHAT_PRIVATE_MSG;
    js["from"] = currentuser.getId();
    js["to"] = stoi(input.substr(0, idx));
    js["msg"] = input.substr(idx + 1, input.size() - idx);
    conn->sendData(js.dump());
}
void addfriend(SocketConn *conn, string input)
{
    json js;
    js["msgid"] = EnMsgType::FRIEND_ADD_MSG;
    js["id"] = currentuser.getId();
    js["friendid"] = stoi(input);
    conn->sendData(js.dump());
}
void joingroup(SocketConn *conn, string input)
{
    json js;
    js["msgid"] = EnMsgType::GROUP_JOIN_MSG;
    js["id"] = currentuser.getId();
    js["groupid"] = stoi(input);
    conn->sendData(js.dump());
}
void creategroup(SocketConn *conn, string input)
{
    int idx;
    if ((idx = input.find(":")) == -1)
    {
        cerr << "invalid input commend!" << endl;
        return;
    }
    json js;
    js["msgid"] = EnMsgType::GROUP_CREATE_MSG;
    js["name"] = input.substr(0, idx);
    js["desc"] = input.substr(idx + 1, input.size() - idx);
    conn->sendData(js.dump());
}
void groupchat(SocketConn *conn, string input)
{
    int idx;
    if ((idx = input.find(":")) == -1)
    {
        cerr << "invalid input commend!" << endl;
        return;
    }
    json js;
    js["msgid"] = EnMsgType::CHAT_GROUP_MSG;
    js["id"] = currentuser.getId();
    js["groupid"] = stoi(input.substr(0, idx));
    js["msg"] = input.substr(idx + 1, input.size() - idx);
    conn->sendData(js.dump());
}
void quit(SocketConn *conn, string input)
{
    json js;
    js["msgid"] = EnMsgType::QUIT_MSG;
    js["id"] = currentuser.getId();
    conn->sendData(js.dump());
}