#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include <vector>

using namespace std;

class OfflineMessageModel
{
public:
    // 增加
    bool insert(const int, const string&);
    // // 删除
    bool remove(const int);
    // 查询
    vector<string> query(const int);

private:
    // int id;
    // string msg;
};

#endif