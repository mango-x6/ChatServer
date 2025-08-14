#include "friendmodel.hpp"
#include "db/db.h"
#include <iostream>

// 增加
bool FriendModel::insert(const int userid, const int friendid)
{
    char query[1024] = "";
    sprintf(query,
            "INSERT INTO Friend(userid, friendid) VALUES(%d, %d)",
            userid, friendid);
    db::MySQL sql;
    if (sql.connect())
    {
        return sql.update(query);
    }
    return false;
}
// 删除
bool FriendModel::remove(const int userid, const int friendid)
{
    char query[1024] = "";
    sprintf(query,
            "DELETE FROM Friend WHERE userid=%d AND friendid=%d",
            userid, friendid);
    db::MySQL sql;
    if (sql.connect())
    {
        return sql.update(query);
    }
    return false;
}
bool FriendModel::removeAll(const int userid)
{
    char query[1024] = "";
    sprintf(query,
            "DELETE FROM Friend WHERE userid=%d",
            userid);
    db::MySQL sql;
    if (sql.connect())
    {
        return sql.update(query);
    }
    return false;
}
// // 查询
vector<string> FriendModel::query(const int userid)
{
    char query[1024] = "";
    sprintf(query,
            "SELECT u.id, u.name, u.state FROM User u INNER JOIN Friend f ON u.id = f.friendid WHERE f.userid = %d",
            userid);

    db::MySQL sql;
    vector<string> friendVec;
    if (sql.connect())
    {
        MYSQL_RES *res = sql.query(query);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            friendVec.push_back(User(atoi(row[0]),row[1],"",row[2]).toJson().dump());
        }
        mysql_free_result(res);
    }
    return friendVec;
}
// int userid;
// int friendid;