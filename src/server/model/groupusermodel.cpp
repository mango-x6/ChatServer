#include "groupusermodel.hpp"
#include "db/db.h"
#include <iostream>

// 增加
bool GroupUserModel::insert(const int userid, const int groupid, string grouprole)
{
    char query[1024] = "";
    sprintf(query,
            "INSERT INTO GroupUser(groupid, userid, grouprole) VALUES(%d, %d, '%s')",
            groupid, userid, grouprole.c_str());
    db::MySQL sql;
    if (sql.connect())
    {
        return sql.update(query);
    }
    return false;
}
// 删除
bool GroupUserModel::remove(const int userid, const int groupid)
{
    char query[1024] = "";
    sprintf(query,
            "DELETE FROM GroupUser WHERE userid=%d AND groupid=%d",
            userid, groupid);
    db::MySQL sql;
    if (sql.connect())
    {
        return sql.update(query);
    }
    return false;
}
