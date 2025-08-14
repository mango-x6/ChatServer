#ifndef ALLGROUPMODEL_H
#define ALLGROUPMODEL_H

#include "group.hpp"
#include <vector>
#include <string>
#include <memory>

using namespace std;

class AllGroupModel
{
public:
    // 增加
    bool insert(Group &);
    // 删除
    bool remove(const int);
    // 查询
    unique_ptr<Group> query(const int);
    // 查询用户加入的群组
    vector<string> queryGroup(const int);
private:
    // int id;
    // string name;
    // string description;
};

#endif