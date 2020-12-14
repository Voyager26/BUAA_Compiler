//
// Created by Phantom on 2020/11/14.
//
#pragma once
#include <string>
#include <vector>

enum mid_operation {
    Main,   //j main
    PLUSOP, //+
    MINUOP, //-
    MULTOP, //*
    DIVOP,  // /
    LSSOP,  //<
    LEQOP,  //<=
    GREOP,  //>
    GEQOP,  //>=
    BEQOP,  //==
    NEQOP,  //!=
    ASSIGNOP,  //=
    GOTO,  //无条件跳转
    BZ,    //不满足条件跳转
    BNZ,   //满足条件跳转
    PUSH,  //函数调用时参数传递
    CALL,  //函数调用
    RET,   //函数返回语句
    RETVALUE, //有返回值函数返回的结果
    SCAN,  //读
    PRINT, //写
    LABEL, //标号
    CON, //常量
    ARRAY, //数组
    VAR,   //无初始化变量
    VVAR,  //初始化变量
    FUNC,  //函数定义
    PARAM, //函数参数
    GETARRAY,  //取数组的值  t = a[]
    PUTARRAY,  //给数组赋值  a[] = t
    EXIT,  //退出 main最后
};

using namespace std;
class midCode {  //z = x op y
public:
    mid_operation op; // 操作
    string z;     // 结果
    string x;     // 左操作数
    string y;     // 右操作数
    vector<int> arrayNum;   //[0]/[0][1]维度 之后为数组元素
    vector<string>arrayInfo;
    //con/var-名字-类型-值
    midCode(mid_operation op, string z, string x, string y) : op(op), z(z), x(x), y(y) {

    }
    //j main
    midCode(mid_operation op) : op(op){

    }
    void info(string t) {
        arrayInfo.push_back(t);
    }
    void insert(int t) {
        arrayNum.push_back(t);
    }
};
