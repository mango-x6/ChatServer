#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <unordered_map>
#include <functional>
#include <json.hpp>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include "model/usermodel.hpp"
#include "model/friendmodel.hpp"
#include "model/allgroupmodel.hpp"
#include "model/offlinemessagemodel.hpp"
#include "model/groupusermodel.hpp"
#include <mutex>

using namespace muduo;
using namespace muduo::net;
using namespace nlohmann::json_abi_v3_12_0;
using MessageHandler = std::function<void(const TcpConnectionPtr &, json &, Timestamp)>;

class ChatService
{
public:
    // 获取实例（单例模式）
    static ChatService *getInstance();
    // 不执行任何操作
    void error_msg(const TcpConnectionPtr &, json &, Timestamp) {}
    // 登录消息
    void login(const TcpConnectionPtr &, json &, Timestamp);
    // 注册消息
    void reg(const TcpConnectionPtr &, json &, Timestamp);
    // 私聊消息
    void privateChat(const TcpConnectionPtr &, json &, Timestamp);
    // 群聊消息
    void groupChat(const TcpConnectionPtr &, json &, Timestamp);
    // 添加好友消息
    void addFriend(const TcpConnectionPtr &, json &, Timestamp);
    // 删除好友消息
    void removeFriend(const TcpConnectionPtr &, json &, Timestamp);
    // 查询好友消息
    void queryFriend(const TcpConnectionPtr &, json &, Timestamp);
    // 创建群组消息
    void createGroup(const TcpConnectionPtr &, json &, Timestamp);
    // 解散群组消息
    void removeGroup(const TcpConnectionPtr &, json &, Timestamp);
    // 查询群组消息
    void showGroup(const TcpConnectionPtr &, json &, Timestamp);
    // 加入群组消息
    void joinGroup(const TcpConnectionPtr &, json &, Timestamp);
    // 退出群组消息
    void quitGroup(const TcpConnectionPtr &, json &, Timestamp);
    // 查询加入群组消息
    void queryGroup(const TcpConnectionPtr &, json &, Timestamp);

    // 获取msgid对应的处理函数
    MessageHandler getHandler(int);
    // 客户端异常退出
    void clientExceptionClose(const TcpConnectionPtr &);
    // 客户端退出
    void clientClose(const TcpConnectionPtr &, json &, Timestamp);
    // 服务端异常退出
    void reset();

private:
    mutex _mutex;
    // 初始化消息服务
    ChatService();
    // 消息处理map
    std::unordered_map<int, MessageHandler> _msgHandlerMap;
    // 用户连接map
    std::unordered_map<int, TcpConnectionPtr> _userConnMap;
    // 数据操作类
    UserModel _userModel;
    // 离线消息操作类
    OfflineMessageModel _offlineMessageModel;
    // 好友操作类
    FriendModel _friendModel;
    // 群组操作类
    AllGroupModel _allgroupmodel;
    // 群组成员操作类
    GroupUserModel _groupusermodel;
    // 发送消息
    void sendMessage(const TcpConnectionPtr &, json &, Timestamp, const int);
};

#endif