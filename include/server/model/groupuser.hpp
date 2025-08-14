#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"

class GroupUser : public User
{
public:
    GroupUser(int id = -1, string name = "", string password = "", string state = "offline", string role = "normal")
        : User(id, name, password, state), role(role) {}
    void setRole(const string &role) { this->role = role; }
    string getRole() const { return this->role; }

    json toJson()
    {
        json js = this->User::toJson();
        js["grouprole"] = this->role;
        return js;
    }

private:
    string role;
};

#endif