//
// Created by Phantom on 2020/11/14.
//
#pragma once
#include <string>
#include <vector>

enum mid_operation {
    MainOp,   //main:
    PlusOp, //+
    MinuOp, //-
    MultOp, //*
    DivOp,  // /
    LssOp,  //<
    LeqOp,  //<=
    GreOp,  //>
    GeqOp,  //>=
    BeqOp,  //==
    NeqOp,  //!=
    AssignOp,  //=
    GoTo,  //无条件跳转
    BZ,    //不满足条件跳转
    BNZ,   //满足条件跳转
    PushOp,  //函数调用时参数传递
    Call,  //函数调用
    ReturnOp,   //函数返回语句
    ReturnValue, //有返回值函数返回的结果
    ScanOp,  //读
    PrintOp, //写
    Label, //标号
    Con, //常量
    ArrayOp, //数组
    VarOp,   //无初始化变量
    VvarOp,  //初始化变量
    FuncOp,  //函数定义
    ParamOp, //函数参数
    GetArray,  //取数组的值  t = a[]
    PutArray,  //给数组赋值  a[] = t
    Exit,  //退出 main最后
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
extern void outputMidCode();
