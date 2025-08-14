#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <unordered_map>
#include <functional>
#include "json.hpp"
#include "public.hpp"
#include "user.hpp"
#include "group.hpp"

using namespace nlohmann::json_abi_v3_12_0;
using namespace std;

extern User currentuser;
extern vector<User> friendlist;
extern vector<Group> grouplist;

extern unordered_map<int, function<void(const json &)>> showJsonMap;

#endif