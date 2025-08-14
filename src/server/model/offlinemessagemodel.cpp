#include "db/db.h"
#include "offlinemessagemodel.hpp"
#include <iostream>

using namespace std;

// 增加
bool OfflineMessageModel::insert(const int id, const string &msg)
{
    char query[1024] = "";
    sprintf(query,
            "INSERT INTO OfflineMessage(userid, message) VALUES('%d', '%s')",
            id, msg.c_str());
    db::MySQL sql;
    if (sql.connect())
    {
        return sql.update(query);
    }
    return false;
}

// 删除
bool OfflineMessageModel::remove(const int id)
{
    char query[1024] = "";
    sprintf(query,
            "DELETE FROM OfflineMessage WHERE userid=%d",
            id);
    db::MySQL sql;
    if (sql.connect())
    {
        return sql.update(query);
    }
    return false;
}

// 查询
vector<string> OfflineMessageModel::query(const int id)
{
    char query[1024] = "";
    sprintf(query,
            "SELECT message FROM OfflineMessage WHERE userid = %d",
            id);

    db::MySQL sql;
    vector<string> msgs;
    if (sql.connect())
    {
        MYSQL_RES *res = sql.query(query);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            msgs.push_back(row[0]);
        }
        mysql_free_result(res);
    }
    return msgs;
}
