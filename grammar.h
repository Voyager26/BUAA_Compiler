//
// Created by Phantom on 2020/10/10.
//

#pragma once
#ifndef COMPILER_GRAMMAR_H
#define COMPILER_GRAMMAR_H

/*所有要求输出的语法成分*/
//＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
bool String();
//＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
bool program();
//＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
bool constDeclaration();
//＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
bool constDefinition();
//＜无符号整数＞  ::= ＜数字＞｛＜数字＞｝
bool unsignedInt(int flag);
//＜整数＞        ::= ［＋｜－］＜无符号整数＞
bool Int();
//＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞
bool deHead();
//＜常量＞   ::=  ＜整数＞|＜字符＞
bool Const();
//＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
bool varDeclaration();
//＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
bool varDefinition();
//＜变量定义无初始化＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'){,(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
bool varUnInitialDefinition();
//＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞|＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'|＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
bool varInitialDefinition();
//＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
bool valueReturnFunction();
//＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
bool noValueReturnFunction();
//＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
bool compoundStatement();
//＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
bool parameterTable();
//＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
bool mainFunction();
//＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}   //[+|-]只作用于第一个<项>
bool expression();
//＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
bool item();
//＜因子＞    ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'|＜标识符＞'['＜表达式＞']''['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞ ＜标识符＞'['＜表达式＞']'和＜标识符＞'['＜表达式＞']''['＜表达式＞']'中的＜表达式＞只能是整型，下标从0开始
bool factor();
//＜语句＞    ::= ＜循环语句＞｜＜条件语句＞| ＜有返回值函数调用语句＞;  |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜情况语句＞｜＜空＞;|＜返回语句＞; | '{'＜语句列＞'}'          /*测试程序需出现11种语句*/
bool Statement();
//＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞|＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
bool assignStatement();
//＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
bool conditionStatement();
//＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
bool condition();
//＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞| for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
bool loopStatement();
//＜步长＞::= ＜无符号整数＞
bool step();
//＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
bool switchStatement();
//＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
bool caseTable();
//＜情况子语句＞  ::=  case＜常量＞：＜语句＞
bool caseStatement();
//＜缺省＞   ::=  default :＜语句＞
bool defaultStatement();
//＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
bool callValueReturnFunction();
//＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
bool callNoValueReturnFunction();
//＜值参数表＞  ::= ＜表达式＞{,＜表达式＞}｜＜空＞
bool valueParameterTable();
//＜语句列＞   ::= ｛＜语句＞｝
bool statementList();
//＜读语句＞    ::=  scanf '('＜标识符＞{,＜标识符＞}')’
bool readStatement();
//＜写语句＞    ::= printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')’
bool writeStatement();
//＜返回语句＞   ::=  return['('＜表达式＞')']
bool returnStatement();
//关系运算符
bool Relation() ;
//查找vecter中该函数是否有返回值
bool isValueReturn() ;

#endif //COMPILER_GRAMMAR_H
