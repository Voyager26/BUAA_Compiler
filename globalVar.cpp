//
// Created by Phantom on 2020/10/18.
//
#include <iostream>
#include <string>
#include <vector>
#include "globalVar.h"

std::ifstream filein;
std::ofstream fileout;
std::string testfile;

std::string symbolstring[40] = {
        "CONSTTK", "INTTK", "CHARTK", "VOIDTK", "MAINTK", "IFTK", "ELSETK", "SWITCHTK", "CASETK", "DEFAULTTK", "WHILETK",
        "FORTK", "SCANFTK", "PRINTFTK", "RETURNTK", "IDENFR", "INTCON", "CHARCON", "STRCON", "PLUS", "MINU", "MULT", "DIV",
        "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ", "COLON", "ASSIGN", "SEMICN", "COMMA", "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"
};

//保留字
std::string reservedWord[reserved_len] = {
        "const", "int", "char", "void", "main", "if", "else", "switch",
        "case", "default", "while", "for", "scanf", "printf", "return",
};

symbol Symbol;           //类别码枚举变量
std::string SYMBOL;
std::string token;       //当前字符串
std::string inLowerToken;//转化为小写的当前字符串
char ch;                 //当前读取的字符
int num;                 //当前整数数值
int indexs = 0;          //当前指向的源程序位置

std::vector<Symbology> symbology;   //临时存储函数类别的数组