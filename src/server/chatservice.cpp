#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>

using namespace std;
using namespace placeholders;

// 获取实例（单例模式）
ChatService *ChatService::getInstance()
{
    static ChatService service;
    return &service;
}

// 初始化消息服务
ChatService::ChatService()
{
    _msgHandlerMap.insert(std::make_pair(EnMsgType::LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::CHAT_PRIVATE_MSG, std::bind(&ChatService::privateChat, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::CHAT_GROUP_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::FRIEND_ADD_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::FRIEND_DELETE_MSG, std::bind(&ChatService::removeFriend, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::FRIEND_QUERY_MSG, std::bind(&ChatService::queryFriend, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::GROUP_CREATE_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::GROUP_REMOVE_MSG, std::bind(&ChatService::removeGroup, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::GROUP_SHOW_MSG, std::bind(&ChatService::showGroup, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::GROUP_JOIN_MSG, std::bind(&ChatService::joinGroup, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::GROUP_QUIT_MSG, std::bind(&ChatService::quitGroup, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::USER_GROUP_QUERY_MSG, std::bind(&ChatService::queryGroup, this, _1, _2, _3)));
    _msgHandlerMap.insert(std::make_pair(EnMsgType::QUIT_MSG, std::bind(&ChatService::clientClose, this, _1, _2, _3)));
}

// 获取msgid对应的处理函数
MessageHandler ChatService::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp t)
        {
            LOG_ERROR << "Can not find handler with msgid: " << msgid;
        };
        LOG_INFO << "find handler with msgid: " << msgid;
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

// 登录消息
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    unique_ptr<User> user = _userModel.query(js["id"].get<int>());
    json response;
    response["msgid"] = EnMsgType::LOGIN_MSG_ACK;
    if (user->getId() == -1)
    {
        response["errno"] = 1;
        response["errmsg"] = "该用户不存在！";
    }
    else if (user->getPassword() != js["password"])
    {
        response["errno"] = 1;
        response["errmsg"] = "密码错误！";
    }
    else if (user->getState() == "online")
    {
        response["errno"] = 1;
        response["errmsg"] = "该用户已登录，请勿重复登录！";
    }
    else if (!_userModel.updateState(user.get()))
    {
        response["errno"] = 1;
        response["errmsg"] = "修改用户登录状态失败！";
    }
    else
    {
        {
            lock_guard<mutex> lock(_mutex);
            _userConnMap.insert(make_pair(user->getId(), conn));
        }
        response["errno"] = 0;
        response["id"] = user->getId();
        response["name"] = user->getName();
        response["offlinemsg"] = _offlineMessageModel.query(user->getId());
        _offlineMessageModel.remove(user->getId());
        response["friends"] = _friendModel.query(user->getId());
        response["groups"] = _allgroupmodel.queryGroup(user->getId());
    }
    conn->send(response.dump());
}

// 注册消息
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    User user(-1, js["name"].get<string>(), js["password"].get<string>());
    auto state = _userModel.insert(user);

    json response;
    response["msgid"] = EnMsgType::REG_MSG_ACK;
    response["errno"] = state ? 0 : 1;
    response["id"] = user.getId();
    conn->send(response.dump());
}

// 客户端异常退出
void ChatService::clientExceptionClose(const TcpConnectionPtr &conn)
{
    User user(-1, "", "", "online");
    {
        lock_guard<mutex> lock(_mutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    _userModel.updateState(&user);
}
// 客户端退出
void ChatService::clientClose(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    User user(js["id"].get<int>(), "", "", "online");
    {
        lock_guard<mutex> lock(_mutex);
        auto it = _userConnMap.find(user.getId());
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }
    _userModel.updateState(&user);
}
// 发送消息
void ChatService::sendMessage(const TcpConnectionPtr &conn, json &js, Timestamp t, const int toid)
{
    // 查找对方是否在线
    {
        lock_guard<mutex> lock(_mutex);
        auto it = _userConnMap.find(toid);

        if (it != _userConnMap.end()) // 即时消息
        {
            it->second->send(js.dump());
            return;
        }
    }
    // 离线消息
    _offlineMessageModel.insert(toid, js.dump());
}

// 私聊消息
void ChatService::privateChat(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    sendMessage(conn, js, t, js["to"].get<int>());
}

// 群聊消息
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    int groupid = js["groupid"].get<int>();
    unique_ptr<Group> group = _allgroupmodel.query(groupid);

    vector<json> users = group->getUsers();
    for (json user : users)
    {
        if (user["id"].get<int>() == js["id"])
        {
            continue;
        }
        sendMessage(conn, js, t, user["id"].get<int>());
    }
}

void ChatService::reset()
{
    _userModel.resetState();
}

// 添加好友消息
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    _friendModel.insert(js["id"].get<int>(), js["friendid"].get<int>());
}

// 删除好友消息
void ChatService::removeFriend(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    _friendModel.remove(js["id"].get<int>(), js["friendid"].get<int>());
}

// 查询好友消息
void ChatService::queryFriend(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    json response;
    response["msgid"] = EnMsgType::FRIEND_QUERY_MSG_ACK;
    response["errno"] = 0;
    response["id"] = js["id"].get<int>();
    response["friends"] = _friendModel.query(js["id"].get<int>());
    conn->send(response.dump());
}

// 创建群组消息
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    Group group(-1, js["name"].get<string>(), js["desc"].get<string>());
    auto state = _allgroupmodel.insert(group);

    json response;
    response["msgid"] = EnMsgType::GROUP_CREATE_MSG_ACK;
    response["errno"] = state ? 0 : 1;
    response["id"] = group.getId();
    conn->send(response.dump());
}

// 解散群组消息
void ChatService::removeGroup(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    _allgroupmodel.remove(js["id"].get<int>());
}

// 查询群组消息
void ChatService::showGroup(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    unique_ptr<Group> group = _allgroupmodel.query(js["id"].get<int>());

    json response;
    response["msgid"] = EnMsgType::GROUP_SHOW_MSG_ACK;
    if (group->getId() == -1)
    {
        response["errno"] = 1;
        response["id"] = js["id"].get<int>();
        response["errmsg"] = "群组不存在！";
    }
    else
    {
        response["errno"] = 0;
        response["id"] = group->getId();
        response["name"] = group->getName();
        response["desc"] = group->getDescription();
        response["users"] = group->getUsers();
    }
    conn->send(response.dump());
}

// 加入群组消息
void ChatService::joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    auto state = _groupusermodel.insert(js["id"].get<int>(), js["groupid"].get<int>());

    json response;
    response["msgid"] = EnMsgType::GROUP_JOIN_MSG_ACK;
    response["errno"] = state ? 0 : 1;
    conn->send(response.dump());
}

// 退出群组消息
void ChatService::quitGroup(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    auto state = _groupusermodel.remove(js["id"].get<int>(), js["groupid"].get<int>());

    json response;
    response["msgid"] = EnMsgType::GROUP_QUIT_MSG_ACK;
    response["errno"] = state ? 0 : 1;
    conn->send(response.dump());
}

// 查询加入群组消息
void ChatService::queryGroup(const TcpConnectionPtr &conn, json &js, Timestamp t)
{
    auto groups = _allgroupmodel.queryGroup(js["id"].get<int>());

    json response;
    response["msgid"] = EnMsgType::USER_GROUP_QUERY_MSG_ACK;
    response["errno"] = groups.size() > 0 ? 0 : 1;
    response["groups"] = groups;
    conn->send(response.dump());
}