//
// Created by Phantom on 2020/10/10.
//
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "Symbology.h"
#include "midCode.h"

#define reserved_len 15

extern std::ifstream filein;
extern std::ofstream fileout;
extern std::ofstream fileerror;
extern std::ofstream mips;
extern std::string testfile;
extern std::ofstream midCodefile;;
enum symbol {
    CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK, IFTK, ELSETK, SWITCHTK, CASETK, DEFAULTTK, WHILETK,
    FORTK, SCANFTK, PRINTFTK, RETURNTK, IDENFR, INTCON, CHARCON, STRCON, PLUS, MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ, COLON, ASSIGN,
    SEMICN, COMMA, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE, null
};

extern std::string symbolstring[];

//保留字
extern std::string reservedWord[reserved_len];

extern symbol Symbol;           //类别码枚举变量
extern std::string token;       //当前字符串
extern std::string inLowerToken;//转化为小写的当前字符串
extern char ch;                 //当前读取的字符
extern char con_ch;
extern int num;                 //当前整数数值
extern int indexs;              //当前指向的源程序位置
extern int oldIndexs;       //上一单词位置
extern int lines;               //当前行号

extern std::vector<midCode> midCodeTable;   //存储中间代码
extern std::map<std::string, Symbology> globalSymbolMap;
extern std::map<std::string, Symbology> localSymbolMap;
extern std::map<string, map<string, Symbology>> allLocalSymbolTable;
extern std::vector<string> stringList;  //保存所有的字符串
void error();
std::string toString(int a);
int toInt(string s);