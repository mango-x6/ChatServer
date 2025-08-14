#include "allgroupmodel.hpp"
#include "db/db.h"
#include <iostream>

// 增加
bool AllGroupModel::insert(Group &group)
{
    char query[1024] = "";
    sprintf(query,
            "INSERT INTO AllGroup(groupname, groupdesc) VALUES('%s', '%s')",
            group.getName().c_str(), group.getDescription().c_str());
    db::MySQL sql;
    if (sql.connect())
    {
        if (sql.update(query))
        {
            group.setId(mysql_insert_id(sql.getConnection()));
            return true;
        }
    }
    return false;
}
// 删除
bool AllGroupModel::remove(const int id)
{
    char query[1024] = "";
    sprintf(query,
            "DELETE FROM AllGroup WHERE id=%d",
            id);
    db::MySQL sql;
    if (sql.connect())
    {
        return sql.update(query);
    }
    return false;
}
// 查询
unique_ptr<Group> AllGroupModel::query(const int id)
{
    char query1[1024] = "";
    sprintf(query1,
            "select * from AllGroup where id=%d;",
            id);
    char query2[1024] = "";
    sprintf(query2,
            "select userid,name,state,grouprole from User u right join GroupUser g on u.id=g.userid where g.groupid=%d;",
            id);
    db::MySQL sql;
    if (sql.connect())
    {
        MYSQL_RES *res = sql.query(query1);
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row != nullptr)
        {
            Group *group = new Group(atoi(row[0]), row[1], row[2]);
            mysql_free_result(res);
            MYSQL_RES *res2 = sql.query(query2);
            while ((row = mysql_fetch_row(res2)) != nullptr)
            {
                auto u = GroupUser(atoi(row[0]), row[1], "", row[2], row[3]).toJson();
                LOG_INFO << u.dump();
                group->getUsers().push_back(u);
            }
            mysql_free_result(res2);
            return unique_ptr<Group>(group);
        }
        mysql_free_result(res);
    }
    return make_unique<Group>();
}
// 查询用户加入的群组
vector<string> AllGroupModel::queryGroup(const int id)
{
    char query1[1024] = "";
    sprintf(query1,
            "select groupid,groupname,groupdesc,grouprole from GroupUser g LEFT JOIN AllGroup a on g.groupid=a.id where g.userid=%d;",
            id);
    vector<string> groups;
    db::MySQL sql;
    if (sql.connect())
    {
        MYSQL_RES *res = sql.query(query1);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            json js;
            js["groupid"] = atoi(row[0]);
            js["groupname"] = row[1];
            js["groupdesc"] = row[2];
            js["grouprole"] = row[3];
            groups.push_back(js.dump());
        }
        mysql_free_result(res);
    }
    return groups;
    ;
}