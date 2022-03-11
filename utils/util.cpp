//
// Created by lenovo on 2022/3/9.
//
#include "util.h"

void string_split(const string& str, const string& splits, vector<string>& res)
{
    if (str == "")		return;
    //在字符串末尾也加入分隔符，方便截取最后一段
    string strs = str + splits;
    size_t pos = strs.find(splits);
    int step = splits.size();

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != strs.npos)
    {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splits);
    }
}

float attribute_split(string str){
    vector<string> str_tokens;
    string_split(str, "=", str_tokens);
    return stof(str_tokens[1]);
}

int startsWith(string s, string sub){
    return s.find(sub) == 0 ? 1 : 0;
}

int endsWith(string s,string sub){
    return s.rfind(sub) == (s.length()-sub.length()) ? 1 : 0;
}


