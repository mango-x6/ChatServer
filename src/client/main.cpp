#include <unistd.h>
#include <thread>
#include <sys/types.h>
#include <ctime>

#include "utils.hpp"
#include "mainmenu.hpp"
#include "socket.hpp"

User currentuser;
vector<User> friendlist;
vector<Group> grouplist;

unordered_map<int, function<void(const json &)>> showJsonMap{
    {EnMsgType::CHAT_PRIVATE_MSG, [](const json &js)
     { cout << "Message from user " << js["from"] << ": " << js["msg"] << endl; }},
    {EnMsgType::CHAT_GROUP_MSG, [](const json &js)
     { cout << "Message from group " << js["groupid"] << " user " << js["id"] << ": " << js["msg"] << endl; }},
    {EnMsgType::REG_MSG_ACK, [](const json &js)
     {
         if (0 != js["errno"].get<int>())
         {
             cerr << "is already exist!" << endl;
         }
         else
         {
             cout << "register success! User Id: " << js["id"] << endl;
         }
     }},

};

// 枚举类型：主菜单选择项
enum EnMainChoice
{
    CHOICE_LOGIN = 1, // 登录选项
    CHOICE_REG,       // 注册选项（自动赋值为2）
    CHOICE_QUIT,      // 退出选项（自动赋值为3）
};

void readTaskHandler(SocketConn *);
void mainMenu(SocketConn *);
// 登录功能函数
void login(SocketConn *, bool);
// 退出功能函数
void quit(SocketConn *);
// 注册功能函数
void reg(SocketConn *);

void showPrivateMsg(const json &);

int main(int argc, char **argv)
{
    // 参数校验（需要IP地址和端口号两个参数）
    if (argc < 3)
    {
        cerr << "command invalid!" << endl;
        exit(1);
    }

    SocketConn *conn = SocketConn::getInstance();
    conn->setIp(argv[1]);         // 第一个参数：服务器IP
    conn->setPort(atoi(argv[2])); // 第二个参数：服务器端口

    if (!conn->start())
    {
        cerr << "socket connect error!" << endl;
        exit(1);
    }

    bool firstin = true;
    // 主循环：持续显示菜单并处理用户选择
    while (1)
    {
        // 打印菜单界面
        cout << "==============================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "==============================" << endl;
        cout << "choice: " << endl;

        // 获取用户输入
        int choice = 0;
        cin >> choice;
        cin.get(); // 吸收换行符

        // 根据用户选择执行对应功能
        switch (choice)
        {
        case EnMainChoice::CHOICE_LOGIN:
            login(conn, firstin); // 调用登录函数
            break;
        case EnMainChoice::CHOICE_REG:
            reg(conn); // 调用注册函数
            break;
        case EnMainChoice::CHOICE_QUIT:
            quit(conn); // 调用退出函数
            break;
        default:
            cerr << "input invalid!" << endl; // 处理非法输入
            break;
        }

        firstin = false;
    }

    return 0;
}

void login(SocketConn *conn, bool firstin)
{
    int id;
    char password[50];

    cout << "User Id: ";
    cin >> id;
    cin.get();
    cout << "Password: ";
    cin.getline(password, 50);

    json js;
    js["msgid"] = EnMsgType::LOGIN_MSG;
    js["id"] = id;
    js["password"] = password;

    string buffer;
    if (conn->sendAndReceive(js.dump(), buffer))
    {
        json response = json::parse(buffer);
        if (0 != response["errno"].get<int>())
        {
            cerr << response["errmsg"] << endl;
        }
        else
        {
            cout << "==============================" << endl;
            json j;
            cout << response["id"] << " login success!" << endl;
            currentuser = User(response["id"].get<int>(), response["name"], js["password"], "online");
            cout << "- Offline Messages:" << endl;
            auto handler = showJsonMap.end();
            for (string s : response["offlinemsg"])
            {
                j = json::parse(s);
                if ((handler = showJsonMap.find(j["msgid"].get<int>())) != showJsonMap.end())
                {
                    (handler->second)(j);
                }
            }
            cout << "- friends:" << endl;
            friendlist.clear();
            for (string s : response["friends"])
            {
                j = json::parse(s);
                cout << j["id"] << "\t" << j["name"] << "\t" << j["state"] << endl;
                friendlist.push_back(User(j["id"].get<int>(), j["name"], "", j["state"]));
            }
            cout << "- joined groups: " << endl;
            grouplist.clear();
            for (string s : response["groups"])
            {
                j = json::parse(s);
                cout << j["groupid"] << "\t" << j["groupname"] << "\t" << j["grouprole"] << endl;
                grouplist.push_back(Group(j["groupid"].get<int>(), j["groupname"], j["groupdesc"]));
            }
            cout << "==============================" << endl;

            if (firstin)
            {
                thread readTask(readTaskHandler, conn);
                readTask.detach();
            }

            mainMenu(conn);
        }
    }
}

// 注册功能函数
void reg(SocketConn *conn)
{
    char password[50], name[50];

    cout << "User Name: ";
    cin.getline(name, 50);
    cout << "Password: ";
    cin.getline(password, 50);

    json js;
    js["msgid"] = EnMsgType::REG_MSG;
    js["name"] = name;
    js["password"] = password;

    string buffer;
    if (conn->sendAndReceive(js.dump(), buffer))
    {
        json response = json::parse(buffer);
        auto handler = showJsonMap.find(js["msgid"].get<int>());
        if (handler != showJsonMap.end())
        {
            (handler->second)(response);
        }
    }
}

// 退出功能函数
void quit(SocketConn *conn)
{
    conn->closeConn();
    exit(0); // 退出程序
}

void readTaskHandler(SocketConn *conn)
{
    while (1)
    {
        string response;
        if (conn->receiveData(response))
        {
            json js = json::parse(response);
            auto handler = showJsonMap.find(js["msgid"].get<int>());
            if (handler != showJsonMap.end())
            {
                (handler->second)(js);
            }
            else
            {
                cout << response << endl;
            }
        }
    }
}
void mainMenu(SocketConn *conn)
{
    MainMenu *menu = MainMenu::getInstance();
    menu->setSocket(conn);

    menu->getHandler("help")("");
    cout << "==============================" << endl;

    string commandbuf, command;
    int idx;
    bool quit = false;
    while (!quit)
    {
        cin >> commandbuf;
        if ((idx = commandbuf.find(":")) == -1)
        {
            command = commandbuf;
            menu->getHandler(command)("");
            quit = (command == "quit");
        }
        else
        {
            command = commandbuf.substr(0, idx);
            menu->getHandler(command)(commandbuf.substr(idx + 1, commandbuf.size() - idx));
        }
    }
}