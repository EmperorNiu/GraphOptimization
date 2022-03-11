//
// Created by lenovo on 2022/3/9.
//

#ifndef PIPEDREAM_UTIL_H
#define PIPEDREAM_UTIL_H
#include <string>
#include <vector>
using namespace std;

void string_split(const string& str, const string& splits, vector<string>& res);
float attribute_split(string str);
int startsWith(string s, string sub);
int endsWith(string s,string sub);

#endif //PIPEDREAM_UTIL_H
