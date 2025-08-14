#ifndef GROUPUSERMODEL_H
#define GROUPUSERMODEL_H

#include "group.hpp"
#include "groupuser.hpp"
#include <vector>
#include <string>

using namespace std;

class GroupUserModel
{
public:
    // 增加
    bool insert(const int, const int, string = "normal");
    // 删除
    bool remove(const int, const int);

private:
    // int userid;
    // int friendid;
};

#endif