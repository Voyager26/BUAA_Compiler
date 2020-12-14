//
// Created by Phantom on 2020/10/29.
//

#include <string>
#include <vector>

#ifndef COMPILER_SYMBOLOGY_H
#define COMPILER_SYMBOLOGY_H

using namespace std;
class Symbology {
public:
    string name;
    string kind;    //var const array fun
    string type;    //void int char
    int constValue; //常量值
    int addr;       //存储到的地址
    int length;
    vector<int> otherInfo;  //如果是函数则为参数表int-0,char-1，如果为数组则为维数-一维元素大小-二维元素大小
    Symbology(string name, string kind, string type, vector<int> other, int addr)  {
        this->name.assign(name);
        this->kind.assign(kind);
        this->type.assign(type);
        this->otherInfo = other;
        this->addr = addr;
    }
    Symbology(string name, string kind, string type, int value, int addr)  {
        this->name.assign(name);
        this->kind.assign(kind);
        this->type.assign(type);
        this->constValue = value;
        this->addr = addr;
    }
    Symbology(string name, string kind, string type, int length = 0)  {
        this->name.assign(name);
        this->kind.assign(kind);
        this->type.assign(type);
        this->length = length;
    }
    Symbology()  {
    }
    void insert(int t) {
        otherInfo.push_back(t);
    }
};


#endif //COMPILER_SYMBOLOGY_H
