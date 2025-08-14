#include "db/db.h"
#include "usermodel.hpp"
#include <iostream>
#include <memory>

using namespace std;

// 增加
bool UserModel::insert(User &user)
{
    char query[1024] = "";
    sprintf(query,
            "INSERT INTO User(name, password, state) VALUES('%s', '%s', '%s')",
            user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    db::MySQL sql;
    if (sql.connect())
    {
        if (sql.update(query))
        {
            user.setId(mysql_insert_id(sql.getConnection()));
            return true;
        }
    }
    return false;
}

// 删除
bool UserModel::remove(const int id)
{
    char query[1024] = "";
    sprintf(query,
            "DELETE FROM User WHERE id=%d",
            id);
    db::MySQL sql;
    if (sql.connect())
    {
        return sql.update(query);
    }
    return false;
}

// 修改
bool UserModel::updateState(const User *user)
{
    if (user->getId() == -1)
    {
        return false;
    }
    char query[1024] = "";
    string newstate = (user->getState() == "online") ? "offline" : "online";
    sprintf(query,
            "UPDATE User SET state = '%s' WHERE id = %d",
            newstate.c_str(), user->getId());

    db::MySQL sql;
    if (sql.connect())
    {
        if (sql.update(query))
        {
            return true;
        }
    }
    return false;
}

void UserModel::resetState()
{
    char query[1024] = "";
    sprintf(query, "UPDATE User SET state = 'offline'");

    db::MySQL sql;
    if (sql.connect())
    {
        sql.update(query);
    }
}

// 查询
unique_ptr<User> UserModel::query(int id)
{
    char query[1024] = "";
    sprintf(query,
            "SELECT * FROM User WHERE id = %d",
            id);

    db::MySQL sql;
    if (sql.connect())
    {
        MYSQL_RES *res = sql.query(query);
        auto row = mysql_fetch_row(res);
        if (row != nullptr)
        {
            User *user = new User(atoi(row[0]), row[1], row[2], row[3]);
            mysql_free_result(res);
            return unique_ptr<User>(user);
        }
        mysql_free_result(res);
    }
    return make_unique<User>();
}
