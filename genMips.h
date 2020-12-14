//
// Created by Phantom on 2020/11/15.
//

#ifndef COMPILER_GENMIPS_H
#define COMPILER_GENMIPS_H

#include <string>
using namespace std;

enum mipsOperation {
    add,
    addu,
    addi,
    sub,
    mult,
    sll,
    divop,
    mflo,
    mfhi,
    beq,
    bne,
    bgt, //扩展指令 相当于一条ALU类指令+一条branch指令
    bge, //扩展指令 相当于一条ALU类指令+一条branch指令
    blt, //扩展指令 相当于一条ALU类指令+一条branch指令
    ble, //扩展指令 相当于一条ALU类指令+一条branch指令
    j,
    jr,
    jal,
    lw,
    sw,
    syscall,
    li,
    la,
    moveop,
    dataSeg,  //.data
    textSeg,  //.text
    asciizSeg,  //.asciiz
    globlSeg,  //.globl
    label,  //产生标号
};

class mipsCode {
public:
    mipsOperation op; // 操作
    string z;     // 结果
    string x;     // 左操作数
    string y;     // 右操作数
    int imme;     // 立即数
    mipsCode(mipsOperation op, string z, string x, string y, int i = 0) : op(op), z(z), x(x), y(y), imme(i) {}
};

void genMips();

void outputMipsCode();

void loadValue(string name, string regName, bool gene, int& va, bool& get);

void storeValue(string name, string regName);

#endif //COMPILER_GENMIPS_H
