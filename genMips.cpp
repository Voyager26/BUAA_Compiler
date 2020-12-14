//
// Created by Phantom on 2020/11/15.
//



#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include "genMips.h"
#include "midCode.h"
#include "globalVar.h"

using namespace std;

vector<mipsCode> mipsCodeTable;
string curFuncName = "";

extern vector<string> stringList;  //保存所有的字符串
extern vector<midCode> midCodeTable;
extern std::map<string, map<string, Symbology>> allLocalSymbolTable;

void loadValue(string name, string regName, bool gene, int &va, bool &get) {
    int addr;
    auto a = allLocalSymbolTable[curFuncName];
    if (allLocalSymbolTable[curFuncName].find(name) != allLocalSymbolTable[curFuncName].end()) {
        if (allLocalSymbolTable[curFuncName][name].kind.compare("const") == 0) {  //const
            va = allLocalSymbolTable[curFuncName][name].constValue;
            if (gene) {
                mipsCodeTable.push_back(mipsCode(li, regName, "", "", va));
            }
            get = true;
        } else {  //var
            addr = allLocalSymbolTable[curFuncName][name].addr;
            mipsCodeTable.push_back(mipsCode(lw, regName, "$fp", "", -4 * addr));
        }
    } else if (globalSymbolMap.find(name) != globalSymbolMap.end()) {
        //auto a = allLocalSymbolTable[curFuncName];
        if (globalSymbolMap[name].kind.compare("const") == 0) {  //const
            va = globalSymbolMap[name].constValue;
            if (gene) {
                mipsCodeTable.push_back(mipsCode(li, regName, "", "", va));
            }
            get = true;
        } else {  //var
            addr = globalSymbolMap[name].addr;
            mipsCodeTable.push_back(mipsCode(lw, regName, "$gp", "", addr * 4));
        }
    } else {  //值
        if (name.size() > 0) {  //不能是空串
            if (gene) {
                mipsCodeTable.push_back(mipsCode(li, regName, "", "", toInt(name)));
            }
            va = toInt(name);
            get = true;
        }
    }
}

void storeValue(string name, string regName) {
    int addr;
    //auto a = allLocalSymbolTable[curFuncName];
    if (allLocalSymbolTable[curFuncName].find(name) != allLocalSymbolTable[curFuncName].end()
        && allLocalSymbolTable[curFuncName][name].kind.compare("var") == 0) {    //var
        addr = allLocalSymbolTable[curFuncName][name].addr;
        mipsCodeTable.push_back(mipsCode(sw, regName, "$fp", "", -4 * addr));
    } else if (globalSymbolMap.find(name) != globalSymbolMap.end()
               && globalSymbolMap[name].kind.compare("var") == 0) {
        //auto a = allLocalSymbolTable[curFuncName];
        addr = globalSymbolMap[name].addr;
        mipsCodeTable.push_back(mipsCode(sw, regName, "$gp", "", addr * 4));
    }
}

void genMips() {
    mipsCodeTable.push_back(mipsCode(dataSeg, "", "", ""));  //.data
    //字符串.asciiz
    for (int i = 0; i < stringList.size(); i++) {
        mipsCodeTable.push_back(mipsCode(asciizSeg, "s_" + toString(i), stringList[i], ""));
    }
    mipsCodeTable.push_back(mipsCode(asciizSeg, "nextLine", "\\n", ""));
    //全局变量从$gp向上加
    mipsCodeTable.push_back(mipsCode(textSeg, "", "", ""));  //.text
    bool first = false;
    int len = 0, addr = 0, va = 0, va2 = 0;
    bool get1 = false, get2 = false;
    int paramSize = 0;
    vector<midCode> pushOpStack;
    for (int i = 0; i < midCodeTable.size(); i++) {
        midCode mc = midCodeTable[i];
        midCode mcNext = mc;
        //auto a = allLocalSymbolTable[curFuncName];
        switch (mc.op) {
            case MainOp: {
                if((midCodeTable[i-1]).op != ReturnOp && first){
                    mipsCodeTable.push_back(mipsCode(jr, "$ra", "", ""));
                    first = true;
                }
                first = true;
                mipsCodeTable.push_back(mipsCode(label, "main", "", ""));
                len = globalSymbolMap["main"].length;
                mipsCodeTable.push_back(mipsCode(moveop, "$fp", "$sp", ""));
                mipsCodeTable.push_back(mipsCode(addi, "$sp", "$sp", "", -4 * len - 8));
                curFuncName = "main";  //记录当前的函数名字
                break;
            }
            case AssignOp: {
                //mc.z是局部的变量 或 全局的变量
                //mc.x可能是标识符也可能是数值 $t0
                loadValue(mc.x, "$t0", true, va, get1);
                storeValue(mc.z, "$t0");
                break;
            }
            case PlusOp: {
                get1 = false;
                loadValue(mc.x, "$t0", false, va, get1);
                get2 = false;
                loadValue(mc.y, "$t1", false, va2, get2);
                if (get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(li, "$t2", "", "", va + va2));
                } else if (get1 && !get2) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t2", "$t1", "", va));
                } else if (!get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t2", "$t0", "", va2));
                } else {
                    mipsCodeTable.push_back(mipsCode(addu, "$t2", "$t0", "$t1"));
                }
                storeValue(mc.z, "$t2");
                break;
            }
            case MinuOp: {
                get1 = false;
                loadValue(mc.x, "$t0", false, va, get1);
                get2 = false;
                loadValue(mc.y, "$t1", false, va2, get2);
                if (get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(li, "$t2", "", "", va - va2));
                } else if (get1 && !get2) {  //va - $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t2", "$t1", "", -va));  //$t1-va
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$0", "$t2"));      //0-$t2
                } else if (!get1 && get2) {  //$t0 - va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t2", "$t0", "", -va2));
                } else {
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0", "$t1"));
                }
                storeValue(mc.z, "$t2");
                break;
            }
            case MultOp: {
                loadValue(mc.x, "$t0", true, va, get1);
                loadValue(mc.y, "$t1", true, va2, get2);
                mipsCodeTable.push_back(mipsCode(mult, "$t0", "$t1", ""));
                mipsCodeTable.push_back(mipsCode(mflo, "$t2", "", ""));
                storeValue(mc.z, "$t2");
                break;
            }
            case DivOp: {
                loadValue(mc.x, "$t0", true, va, get1);
                loadValue(mc.y, "$t1", true, va2, get2);
                mipsCodeTable.push_back(mipsCode(divop, "$t0", "$t1", ""));
                mipsCodeTable.push_back(mipsCode(mflo, "$t2", "", ""));
                storeValue(mc.z, "$t2");
                break;
            }
            case ScanOp: {
                //mc.z是局部的变量 或 全局的变量
                if (allLocalSymbolTable[curFuncName].find(mc.z) != allLocalSymbolTable[curFuncName].end()
                    && allLocalSymbolTable[curFuncName][mc.z].kind.compare("var") == 0) {
                    if (allLocalSymbolTable[curFuncName][mc.z].type.compare("int") == 0) {  //int
                        mipsCodeTable.push_back(mipsCode(li, "$v0", "", "", 5));
                    } else {  //char
                        mipsCodeTable.push_back(mipsCode(li, "$v0", "", "", 12));
                    }
                    mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
                    addr = allLocalSymbolTable[curFuncName][mc.z].addr;
                    mipsCodeTable.push_back(mipsCode(sw, "$v0", "$fp", "", -4 * addr));
                } else if (globalSymbolMap.find(mc.z) != globalSymbolMap.end()
                           && globalSymbolMap[mc.z].kind.compare("var") == 0) {
                    if (globalSymbolMap[mc.z].type.compare("int") == 0) {  //int
                        mipsCodeTable.push_back(mipsCode(li, "$v0", "", "", 5));
                    } else {  //char
                        mipsCodeTable.push_back(mipsCode(li, "$v0", "", "", 12));
                    }
                    mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
                    addr = globalSymbolMap[mc.z].addr;
                    mipsCodeTable.push_back(mipsCode(sw, "$v0", "$gp", "", addr * 4));
                }
                break;
            }
            case PrintOp: {
                if (mc.x[0] == '0') {  //string
                    for (int i = 0; i < stringList.size(); i++) {
                        if (stringList[i] == mc.z) {
                            mipsCodeTable.push_back(mipsCode(la, "$a0", "s_" + toString(i), ""));
                            break;
                        }
                    }
                    mipsCodeTable.push_back(mipsCode(li, "$v0", "", "", 4));
                    mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
                } else if (mc.x[0] == '1') {  //换行
                    mipsCodeTable.push_back(mipsCode(la, "$a0", "nextLine", ""));
                    mipsCodeTable.push_back(mipsCode(li, "$v0", "", "", 4));
                    mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
                } else { //int char
                    loadValue(mc.z, "$a0", true, va, get1);
                    mipsCodeTable.push_back(mipsCode(li, "$v0", "", "", mc.x[0] == '2' ? 1 : 11));
                    mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
                }
                break;
            }
            case Label: {
                mipsCodeTable.push_back(mipsCode(label, mc.z, "", ""));
                break;
            }
            case FuncOp: {
                if((midCodeTable[i-1]).op != ReturnOp && first){
                    mipsCodeTable.push_back(mipsCode(jr, "$ra", "", "", 10));
                    first = true;
                }
                first = true;
                mipsCodeTable.push_back(mipsCode(label, mc.x, "", ""));
                curFuncName = mc.x;  //记录当前的函数名字
                break;
            }
            case PushOp: {
                pushOpStack.push_back(mc);
                break;
            }
            case Call: {
                // mc.z funName
                paramSize = globalSymbolMap[mc.z].otherInfo.size();
                int h = 1;
                for (int k = paramSize; k > 0; k --) {
                    midCode tmp = pushOpStack[pushOpStack.size()-1];
                    pushOpStack.pop_back();
                    loadValue(tmp.z, "$t0", true, va, get1);
                    mipsCodeTable.push_back(mipsCode(sw, "$t0", "$sp", "", -4 * (2+k)));
                }
                mipsCodeTable.push_back(mipsCode(sw, "$ra", "$sp", "", 0));
                mipsCodeTable.push_back(mipsCode(sw, "$fp", "$sp", "", -4));
                mipsCodeTable.push_back(mipsCode(sw, "$sp", "$sp", "", -8));
                mipsCodeTable.push_back(mipsCode(moveop, "$fp", "$sp", ""));
                mipsCodeTable.push_back(mipsCode(addi, "$sp", "$sp", "", -4 * globalSymbolMap[mc.z].length - 8));
                mipsCodeTable.push_back(mipsCode(jal, mc.z, "", ""));
                mipsCodeTable.push_back(mipsCode(lw, "$ra", "$fp", "", 0));
                mipsCodeTable.push_back(mipsCode(lw, "$fp", "$fp", "", -4));
                mipsCodeTable.push_back(mipsCode(addi, "$sp", "$sp", "", 4 * globalSymbolMap[mc.z].length + 8));
                break;
            }
            case ReturnOp: {
                //0-ra,1-fp, 2...局部变量
                loadValue(mc.z, "$v0", true, va, get1);
                mipsCodeTable.push_back(mipsCode(jr, "$ra", "", ""));
                break;
            }
            case ReturnValue: {
                // 表达式内的函数调用需要把$v0的值赋给mc.z
                if (allLocalSymbolTable[curFuncName].find(mc.z) != allLocalSymbolTable[mc.x].end()
                    && allLocalSymbolTable[curFuncName][mc.z].kind.compare("var") == 0) {
                    addr = allLocalSymbolTable[curFuncName][mc.z].addr;
                    mipsCodeTable.push_back(mipsCode(sw, "$v0", "$fp", "", -4 * addr));
                }
                break;
            }
            case ArrayOp: {
                //op z:name x:类型 y:有无初始化 arrayNum: //[0]维度 [1]/[1][2]各维度个数 之后为数组元素
                bool isGlobal = true;
                if (mc.y.compare("0") == 0) {
                    break;
                }
                int dim = mc.arrayNum[0];
                if (allLocalSymbolTable[curFuncName].find(mc.z) != allLocalSymbolTable[curFuncName].end()
                    && allLocalSymbolTable[curFuncName][mc.z].kind.compare("array") == 0) {  //array
                    addr = allLocalSymbolTable[curFuncName][mc.z].addr;
                    isGlobal = false;
                } else if (globalSymbolMap.find(mc.z) != globalSymbolMap.end()
                           && globalSymbolMap[mc.z].kind.compare("array") == 0) {
                    addr = globalSymbolMap[mc.z].addr;
                    isGlobal = true;
                }
                if (dim == 1) {
                    int dim1 = mc.arrayNum[1];
                    if (isGlobal) {
                        for (int k = 0; k < dim1; k++) {
                            mipsCodeTable.push_back(mipsCode(li, "$t0", "", "", mc.arrayNum[k + 2]));
                            mipsCodeTable.push_back(mipsCode(sw, "$t0", "$gp", "", (addr + k) * 4));
                        }
                    } else {
                        for (int k = 0; k < dim1; k++) {
                            mipsCodeTable.push_back(mipsCode(li, "$t0", "", "", mc.arrayNum[k + 2]));
                            mipsCodeTable.push_back(mipsCode(sw, "$t0", "$fp", "", (addr + k) * -4));
                        }
                    }
                } else {
                    int dim1 = mc.arrayNum[1];
                    int dim2 = mc.arrayNum[2];
                    if (isGlobal) {
                        for (int k = 0; k < dim1; k++) {
                            for (int h = 0; h < dim2; h++) {
                                mipsCodeTable.push_back(mipsCode(li, "$t0", "", "", mc.arrayNum[k * dim2 + h + 3]));
                                mipsCodeTable.push_back(mipsCode(sw, "$t0", "$gp", "", (addr + k * dim2 + h) * 4));
                            }
                        }
                    } else {
                        for (int k = 0; k < dim1; k++) {
                            for (int h = 0; h < dim2; h++) {
                                mipsCodeTable.push_back(mipsCode(li, "$t0", "", "", mc.arrayNum[k * dim2 + h + 3]));
                                mipsCodeTable.push_back(mipsCode(sw, "$t0", "$fp", "", (addr + k * dim2 + h) * -4));
                            }
                        }
                    }
                }
                break;
            }
            case GetArray: {
                if (mc.y.compare("1") == 0) {
                    get1 = false;
                    loadValue(mc.arrayInfo[0], "$t0", false, va, get1);
                    if (allLocalSymbolTable[curFuncName].find(mc.x) != allLocalSymbolTable[curFuncName].end()
                        && allLocalSymbolTable[curFuncName][mc.x].kind.compare("array") == 0) {  //array
                        addr = allLocalSymbolTable[curFuncName][mc.x].addr;
                        if (!get1) {    //为变量，从寄存器中直接操作-4*addr-t0<<4
                            mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                            mipsCodeTable.push_back(mipsCode(addi, "$t1", "$fp", "", -4 * addr));
                            mipsCodeTable.push_back(mipsCode(sub, "$t1", "$t1", "$t0"));
                            mipsCodeTable.push_back(mipsCode(lw, "$t0", "$t1", "", 0));
                        } else {          //直接为数字
                            mipsCodeTable.push_back(mipsCode(lw, "$t0", "$fp", "", -4 * (addr + va)));
                        }
                    } else if (globalSymbolMap.find(mc.x) != globalSymbolMap.end()
                               && globalSymbolMap[mc.x].kind.compare("array") == 0) {  //array
                        addr = globalSymbolMap[mc.x].addr;
                        if (!get1) {
                            mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                            mipsCodeTable.push_back(mipsCode(addi, "$t1", "$gp", "", addr * 4));
                            mipsCodeTable.push_back(mipsCode(add, "$t1", "$t1", "$t0"));
                            mipsCodeTable.push_back(mipsCode(lw, "$t0", "$t1", "", 0));
                        } else {
                            mipsCodeTable.push_back(mipsCode(lw, "$t0", "$gp", "", (addr + va) * 4));
                        }
                    }
                } else {
                    int va1 = 0, va2 = 0, dim1 = 0, dim2 = 0;
                    get1 = false;
                    loadValue(mc.arrayInfo[0], "$t0", false, va1, get1);    //一维下标
                    get2 = false;
                    loadValue(mc.arrayInfo[1], "$t1", false, va2, get2);    //二维下标
                    if (allLocalSymbolTable[curFuncName].find(mc.x) != allLocalSymbolTable[curFuncName].end()
                        && allLocalSymbolTable[curFuncName][mc.x].kind.compare("array") == 0) {  //array
                        addr = allLocalSymbolTable[curFuncName][mc.x].addr;
                        dim2 = allLocalSymbolTable[curFuncName][mc.x].otherInfo[2];
                        if (get1) {    //为变量，从寄存器中直接操作t0-1
                            mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                        }
                        if (get2) {    //为变量直接使用t1，不是变量将va2赋给t1
                            mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                        }
                        mipsCodeTable.push_back(mipsCode(addi, "$t2", "$0", "", dim2));
                        mipsCodeTable.push_back(mipsCode(mult, "$t0", "$t2", ""));
                        mipsCodeTable.push_back(mipsCode(mflo, "$t2", "", ""));
                        mipsCodeTable.push_back(mipsCode(add, "$t0", "$t2", "$t1"));
                        mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                        mipsCodeTable.push_back(mipsCode(addi, "$t1", "$fp", "", -4 * addr));
                        mipsCodeTable.push_back(mipsCode(sub, "$t1", "$t1", "$t0"));
                        mipsCodeTable.push_back(mipsCode(lw, "$t0", "$t1", "", 0));
                    } else if (globalSymbolMap.find(mc.x) != globalSymbolMap.end()
                               && globalSymbolMap[mc.x].kind.compare("array") == 0) {  //array
                        addr = globalSymbolMap[mc.x].addr;
                        dim2 = globalSymbolMap[mc.x].otherInfo[2];
                        if (get1) {    //为变量，从寄存器中直接操作t0-1
                            mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                        }
                        if (get2) {    //为变量直接使用t1，不是变量将va2赋给t1
                            mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                        }
                        mipsCodeTable.push_back(mipsCode(addi, "$t2", "$0", "", dim2));
                        mipsCodeTable.push_back(mipsCode(mult, "$t0", "$t2", ""));
                        mipsCodeTable.push_back(mipsCode(mflo, "$t2", "", ""));
                        mipsCodeTable.push_back(mipsCode(add, "$t0", "$t2", "$t1"));
                        mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                        mipsCodeTable.push_back(mipsCode(addi, "$t1", "$gp", "", addr * 4));
                        mipsCodeTable.push_back(mipsCode(add, "$t1", "$t1", "$t0"));
                        mipsCodeTable.push_back(mipsCode(lw, "$t0", "$t1", "", 0));
                    }
                }
                storeValue(mc.z, "$t0");
                break;
            }
            case PutArray: {
                //z-要存入的数 x-name y-维数
                get1 = false;
                loadValue(mc.z, "$t3", true, va, get1); //要存入的数
                if (mc.y.compare("1") == 0) {
                    loadValue(mc.arrayInfo[0], "$t0", false, va, get1);
                    if (allLocalSymbolTable[curFuncName].find(mc.x) != allLocalSymbolTable[curFuncName].end()
                        && allLocalSymbolTable[curFuncName][mc.x].kind.compare("array") == 0) {  //array
                        addr = allLocalSymbolTable[curFuncName][mc.x].addr;
                        if (!get1) {
                            mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                            mipsCodeTable.push_back(mipsCode(addi, "$t1", "$fp", "", -4 * addr));
                            mipsCodeTable.push_back(mipsCode(sub, "$t1", "$t1", "$t0"));
                            mipsCodeTable.push_back(mipsCode(sw, "$t3", "$t1", "", 0));
                        } else { //拿到了数组下标 存在了va中
                            mipsCodeTable.push_back(mipsCode(sw, "$t3", "$fp", "", -4 * (addr + va)));
                        }
                    } else if (globalSymbolMap.find(mc.x) != globalSymbolMap.end()
                               && globalSymbolMap[mc.x].kind.compare("array") == 0) {  //array
                        addr = globalSymbolMap[mc.x].addr;
                        if (!get1) {
                            mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                            mipsCodeTable.push_back(mipsCode(addi, "$t1", "$gp", "", addr * 4));
                            mipsCodeTable.push_back(mipsCode(add, "$t1", "$t1", "$t0"));
                            mipsCodeTable.push_back(mipsCode(sw, "$t3", "$t1", "", 0));
                        } else {
                            mipsCodeTable.push_back(mipsCode(sw, "$t3", "$gp", "", (addr + va) * 4));
                        }
                    }
                } else {
                    int va1 = 0, va2 = 0, dim1 = 0, dim2 = 0;
                    get1 = false;
                    loadValue(mc.arrayInfo[0], "$t0", false, va1, get1);    //一维下标
                    get2 = false;
                    loadValue(mc.arrayInfo[1], "$t1", false, va2, get2);    //二维下标
                    if (allLocalSymbolTable[curFuncName].find(mc.x) != allLocalSymbolTable[curFuncName].end()
                        && allLocalSymbolTable[curFuncName][mc.x].kind.compare("array") == 0) {  //array
                        addr = allLocalSymbolTable[curFuncName][mc.x].addr;
                        dim2 = allLocalSymbolTable[curFuncName][mc.x].otherInfo[2];
                        if (get1) {    //为变量，从寄存器中直接操作t0-1
                            mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                        }
                        if (get2) {    //为变量直接使用t1，不是变量将va2赋给t1
                            mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                        }
                        mipsCodeTable.push_back(mipsCode(addi, "$t2", "$0", "", dim2));
                        mipsCodeTable.push_back(mipsCode(mult, "$t0", "$t2", ""));
                        mipsCodeTable.push_back(mipsCode(mflo, "$t2", "", ""));
                        mipsCodeTable.push_back(mipsCode(add, "$t0", "$t2", "$t1"));
                        mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                        mipsCodeTable.push_back(mipsCode(addi, "$t1", "$fp", "", -4 * addr));
                        mipsCodeTable.push_back(mipsCode(sub, "$t1", "$t1", "$t0"));
                        mipsCodeTable.push_back(mipsCode(sw, "$t3", "$t1", "", 0));
                    } else if (globalSymbolMap.find(mc.x) != globalSymbolMap.end()
                               && globalSymbolMap[mc.x].kind.compare("array") == 0) {  //array
                        addr = globalSymbolMap[mc.x].addr;
                        dim2 = globalSymbolMap[mc.x].otherInfo[2];
                        if (get1) {    //为变量，从寄存器中直接操作t0-1
                            mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                        }
                        if (get2) {    //为变量直接使用t1，不是变量将va2赋给t1
                            mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                        }
                        mipsCodeTable.push_back(mipsCode(addi, "$t2", "$0", "", dim2));
                        mipsCodeTable.push_back(mipsCode(mult, "$t0", "$t2", ""));
                        mipsCodeTable.push_back(mipsCode(mflo, "$t2", "", ""));
                        mipsCodeTable.push_back(mipsCode(add, "$t0", "$t2", "$t1"));
                        mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                        mipsCodeTable.push_back(mipsCode(addi, "$t1", "$gp", "", addr * 4));
                        mipsCodeTable.push_back(mipsCode(add, "$t1", "$t1", "$t0"));
                        mipsCodeTable.push_back(mipsCode(sw, "$t3", "$t1", "", 0));
                    }
                }
                break;
            }
            case Exit: {
                mipsCodeTable.push_back(mipsCode(li, "$v0", "", "", 10));
                mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
                break;
            }
            case VvarOp: {
                va = toInt(mc.y);
                bool get;
                mipsCodeTable.push_back(mipsCode(li, "$t0", "", "", va));
                storeValue(mc.z, "$t0");
                break;
            }
            case GoTo: {
                mipsCodeTable.push_back(mipsCode(j, mc.z, "", ""));
                break;
            }
            case LssOp: {  //<
                int va1, va2;
                get1 = false;
                loadValue(mc.x, "$t0", false, va1, get1);
                get2 = false;
                loadValue(mc.y, "$t1", false, va2, get2);
                if (get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(bge, "$t0", "$t1", mc.z));  //bge $t0 $t1 label >=跳转
                } else if (get1 && !get2) {  //va >= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(bge, "$t0", "$t1", mc.z));  //bge >=跳转
                } else if (!get1 && get2) {  //$t0 >= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(bge, "$t0", "$t1", mc.z));  //bge >=跳转
                } else {  //$t0 >= $t1
                    mipsCodeTable.push_back(mipsCode(bge, "$t0", "$t1", mc.z));  //bge >=跳转
                }
                break;
            }
            case LeqOp: {  //<=
                int va1, va2;
                get1 = false;
                loadValue(mc.x, "$t0", false, va1, get1);
                get2 = false;
                loadValue(mc.y, "$t1", false, va2, get2);
                if (get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(bgt, "$t0", "$t1", mc.z));  //bgt $t0 $t1 label >=跳转
                } else if (get1 && !get2) {  //va >= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(bgt, "$t0", "$t1", mc.z));  //bgt >=跳转
                } else if (!get1 && get2) {  //$t0 >= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(bgt, "$t0", "$t1", mc.z));  //bgt >=跳转
                } else {  //$t0 >= $t1
                    mipsCodeTable.push_back(mipsCode(bgt, "$t0", "$t1", mc.z));  //bgt >=跳转
                }
                break;
            }
            case GreOp: {  //>
                int va1, va2;
                get1 = false;
                loadValue(mc.x, "$t0", false, va1, get1);
                get2 = false;
                loadValue(mc.y, "$t1", false, va2, get2);
                if (get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(ble, "$t0", "$t1", mc.z));  //ble $t0 $t1 label <=跳转
                } else if (get1 && !get2) {  //va >= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(ble, "$t0", "$t1", mc.z));  //ble <=跳转
                } else if (!get1 && get2) {  //$t0 >= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(ble, "$t0", "$t1", mc.z));  //ble <=跳转
                } else {  //$t0 >= $t1
                    mipsCodeTable.push_back(mipsCode(ble, "$t0", "$t1", mc.z));  //ble <=跳转
                }
                break;
            }
            case GeqOp: {  //>=
                int va1, va2;
                get1 = false;
                loadValue(mc.x, "$t0", false, va1, get1);
                get2 = false;
                loadValue(mc.y, "$t1", false, va2, get2);
                if (get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(blt, "$t0", "$t1", mc.z));  //blt $t0 $t1 label <跳转
                } else if (get1 && !get2) {  //va >= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(blt, "$t0", "$t1", mc.z));  //<跳转
                } else if (!get1 && get2) {  //$t0 >= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(blt, "$t0", "$t1", mc.z));  //<跳转
                } else {  //$t0 >= $t1
                    mipsCodeTable.push_back(mipsCode(blt, "$t0", "$t1", mc.z));  //<跳转
                }
                break;
            }
            case BeqOp: {  //==
                int va1, va2;
                get1 = false;
                loadValue(mc.x, "$t0", false, va1, get1);
                get2 = false;
                loadValue(mc.y, "$t1", false, va2, get2);
                if (get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mc.z));  //bne $t0 $t1 label !=跳转
                } else if (get1 && !get2) {  //va >= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mc.z));  // !=跳转
                } else if (!get1 && get2) {  //$t0 >= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mc.z));  // !=跳转
                } else {  //$t0 >= $t1
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mc.z));  // !=跳转
                }
                break;
            }
            case NeqOp: {  // !=
                int va1, va2;
                get1 = false;
                loadValue(mc.x, "$t0", false, va1, get1);
                get2 = false;
                loadValue(mc.y, "$t1", false, va2, get2);
                if (get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mc.z));  //beq $t0 $t1 label ==跳转
                } else if (get1 && !get2) {  //va >= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$0", "", va1));
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mc.z));  //==跳转
                } else if (!get1 && get2) {  //$t0 >= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$0", "", va2));
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mc.z));  //==跳转
                } else {  //$t0 >= $t1
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mc.z));  //==跳转
                }
                break;
            }
            default: {
                break;
            }
        }
    }
    if((midCodeTable.end()-1)->op != Exit){
        mipsCodeTable.push_back(mipsCode(li, "$v0", "", "", 10));
        mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
    }
}

void outputMipsCode() {
    for (int i = 0; i < mipsCodeTable.size(); i++) {
        mipsCode mc = mipsCodeTable[i];
        switch (mc.op) {
            case add:
                mips << "add " << mc.z << "," << mc.x << "," << mc.y << "\n";
                break;
            case addu:
                mips << "addu " << mc.z << "," << mc.x << "," << mc.y << "\n";
                break;
            case sub:
                mips << "sub " << mc.z << "," << mc.x << "," << mc.y << "\n";
                break;
            case mult:
                mips << "mult " << mc.z << "," << mc.x << "\n";
                break;
            case divop:
                mips << "div " << mc.z << "," << mc.x << "\n";
                break;
            case addi:
                mips << "addi " << mc.z << "," << mc.x << "," << mc.imme << "\n";
                break;
            case sll:
                mips << "sll " << mc.z << "," << mc.x << "," << mc.imme << "\n";
                break;
            case mflo:
                mips << "mflo " << mc.z << "\n";
                break;
            case mfhi:
                mips << "mfhi " << mc.z << "\n";
            case j:
                mips << "j " << mc.z << "\n";
                break;
            case jr:
                mips << "jr " << mc.z << "\n";
                break;
            case jal:
                mips << "jal " << mc.z << "\n";
                break;
            case lw:
                mips << "lw " << mc.z << "," << mc.imme << "(" << mc.x << ")\n";
                break;
            case sw:
                mips << "sw " << mc.z << "," << mc.imme << "(" << mc.x << ")\n";
                break;
            case syscall:
                mips << "syscall\n";
                break;
            case li:
                mips << "li " << mc.z << "," << mc.imme << "\n";
                break;
            case la:
                mips << "la " << mc.z << "," << mc.x << "\n";
                break;
            case moveop:
                mips << "move " << mc.z << "," << mc.x << "\n";
                break;
            case dataSeg:
                mips << ".data\n";
                break;
            case textSeg:
                mips << ".text\n";
                break;
            case asciizSeg:
                mips << mc.z << ": .asciiz \"" << mc.x << "\"\n";
                break;
            case globlSeg:
                mips << ".globl main\n";
                break;
            case label:
                mips << mc.z << ":\n";
                break;
            case beq:
                mips << "beq " << mc.z << "," << mc.x << "," << mc.y << "\n";
                break;
            case bne:
                mips << "bne " << mc.z << "," << mc.x << "," << mc.y << "\n";
                break;
            case bgt:
                mips << "bgt " << mc.z << "," << mc.x << "," << mc.y << "\n";
                break;
            case bge:
                mips << "bge " << mc.z << "," << mc.x << "," << mc.y << "\n";
                break;
            case blt:
                mips << "blt " << mc.z << "," << mc.x << "," << mc.y << "\n";
                break;
            case ble:
                mips << "ble " << mc.z << "," << mc.x << "," << mc.y << "\n";
                break;
                break;
            default:
                break;
        }
    }
}