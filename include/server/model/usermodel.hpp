#ifndef USERMODEL_H
#define USERMODEL_H

#include <memory>

#include "user.hpp"

class UserModel
{
public:
    // 增加
    bool insert(User &);
    // // 删除
    bool remove(const int id);
    // 修改
    bool updateState(const User *);
    void resetState();
    // // 查询
    unique_ptr<User> query(const int);

private:
    // int id;
    // string name;
    // string password;
    // string state;
};

#endif