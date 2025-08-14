#ifndef USER_H
#define USER_H

#include <string>
#include "json.hpp"

using namespace std;
using namespace nlohmann::json_abi_v3_12_0;

class User
{
public:
    User(int id = -1, string name = "", string password = "", string state = "offline")
        : id(id), name(name), password(password), state(state) {}

    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setPassword(string password) { this->password = password; }
    void setState(string state) { this->password = password; }
    int getId() const { return this->id; }
    string getName() const { return this->name; }
    string getPassword() const { return this->password; }
    string getState() const { return this->state; }
    json toJson()
    {
        json js;
        js["id"] = this->id;
        js["name"] = this->name;
        js["state"] = this->state;
        return js;
    }

protected:
    int id;
    string name;
    string password;
    string state;
};

#endif