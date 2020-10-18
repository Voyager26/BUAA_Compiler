//
// Created by Phantom on 2020/10/10.
//
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define reserved_len 15

extern std::ifstream filein;
extern std::ofstream fileout;
extern std::string testfile;
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
extern int num;                 //当前整数数值
extern int indexs;              //当前指向的源程序位置

typedef struct SYMBOLOGY {
    std::string name;
    int flag;           //0-无返回值，1-有返回值
} Symbology;

extern std::vector<Symbology> symbology;    //临时存储函数类别的数组
void error();
