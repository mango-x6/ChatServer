#ifndef GROUP_H
#define GROUP_H

#include <string>
#include "groupuser.hpp"
#include "json.hpp"

using namespace std;
// using namespace nlohmann::json_abi_v3_12_0;

class Group
{
public:
    Group(int id = -1, string name = "", string description = "")
        : id(id), name(name), description(description) {}

    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setDescription(string description) { this->description = description; }

    int getId() const { return this->id; }
    string getName() const { return this->name; }
    string getDescription() const { return this->description; }
    vector<json> &getUsers() { return this->users; }
    // string show()
    // {
    //     json js;
    //     js["id"] = id;
    //     js["name"] = name;
    //     js["state"] = state;
    //     return js.dump();
    // }

private:
    int id;
    string name;
    string description;
    vector<json> users;
};

#endif