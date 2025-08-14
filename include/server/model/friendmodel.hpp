#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include <vector>
#include <string>

using namespace std;

class FriendModel
{
public:
    // 增加
    bool insert(const int, const int);
    // // 删除
    bool remove(const int, const int);
    bool removeAll(const int);
    // // 查询
    vector<string> query(const int);

private:
    // int userid;
    // int friendid;
};

#endif