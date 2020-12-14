//
// Created by Phantom on 2020/10/18.
//
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <sstream>
#include "Symbology.h"
#include "globalVar.h"
#include "midCode.h"

std::ifstream filein;
std::ofstream fileout;
std::ofstream fileerror;
std::ofstream mips;
std::ofstream midCodefile;
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
char con_ch;
int num;                 //当前整数数值
int indexs = 0;          //当前指向的源程序位置
int oldIndexs = 0;       //上一单词位置
int lines = 1;           //当前行号
std::vector<midCode> midCodeTable;   //存储中间代码
std::map<std::string, Symbology> globalSymbolMap;
std::map<std::string, Symbology> localSymbolMap;
map<string, map<string, Symbology>> allLocalSymbolTable;  //保存所有的局部符号表 用于保留变量的地址
vector<string> stringList;  //保存所有的字符串

std::string toString(int a){
    return std::to_string(a);
}
int toInt(string s) {
    stringstream ss;
    ss << s;
    int t;
    ss >> t;
    return t;
}