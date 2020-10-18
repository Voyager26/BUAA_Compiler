#pragma once
#ifndef COMPILER_LEXICAL_H
#define COMPILER_LEXICAL_H

//类别码
bool isSpace(char in);

bool isEnter(char in) ;

bool isBlank(char in);

bool isLetter(char in);

bool isDigit(char in);

bool isColon(char in) ;

bool isSemicn(char in) ;

bool isComma(char in) ;

bool isEqu(char in) ;

bool isPlus(char in);

bool isMinu(char in) ;

bool isMult(char in);

bool isDiv(char in);

bool isLss(char in);

bool isGre(char in);

bool isExcla(char in);

bool isLpar(char in) ;

bool isRpar(char in) ;

bool isLbracket(char in) ;

bool isRbracket(char in) ;

bool isLbrace(char in) ;

bool isRbrace(char in) ;

bool isSinglequo(char in);

bool isDoublequo(char in);

bool isChar(char in);

/**
 * 查找保留字
 * return 如果找到，保留字下标
 * */
int isReserved() ;

void clearToken() ;

/**
 * 连接当前字符到当前token
 */
void catToken() ;

void catLowerToken() ;

bool isEOF() ;

void retract() ;

int transNum() ;

void getch() ;

int getsym();

void doOutPut();

#endif //COMPILER_LEXICAL_H
