//
// Created by Phantom on 2020/10/10.
//
#include <iostream>
#include <string>
#include "grammar.h"
#include "lexical.h"
#include "globalVar.h"

using namespace std;

//＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
bool String() {
    if (Symbol == STRCON) {
        doOutPut();
        fileout << "<字符串>" << endl;
        getsym();
        return true;
    } else {
        return false;
    }
}

//＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
bool program() {
    constDeclaration();
    varDeclaration();
    while (true) {
        if (Symbol == INTTK || Symbol == CHARTK) {
            if (!valueReturnFunction()) {           //有返回值函数
                return false;                           //不是有返回值函数，则错误
            }
        } else if (Symbol == VOIDTK) {
            if (!noValueReturnFunction()) {             //无返回值函数
                break;                                  //不是无返回值函数，则可能为主函数，因此进入主函数判断程序
            }
        } else {
            return false;
        }
    }
    if (mainFunction()) {                               //主函数判断程序
        if (isEOF()) {
            fileout << "<程序>" << endl;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

//＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
bool constDeclaration() {
    if (Symbol != CONSTTK) {
        return false;
    }
    if (Symbol == CONSTTK) {
        doOutPut();
        getsym();
        if (constDefinition()) {     //是否为常量定义，预读了下一个符号
            if (Symbol == SEMICN) {
                doOutPut();
                getsym();
                while (Symbol == CONSTTK) {     //不是const则常量说明结束
                    doOutPut();
                    getsym();
                    if (!constDefinition()) {
                        error();
                        return false;
                    }
                    if (Symbol != SEMICN) {
                        error();
                        return false;
                    } else {                         //最后为；则正确
                        doOutPut();
                        getsym();
                        continue;
                    }
                }
            } else {
                error();
                return false;
            }
        }
    }
    fileout << "<常量说明>" << endl;
    return true;
}

//＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
bool constDefinition() {
    if (Symbol == INTTK) {
        doOutPut();
        getsym();
        if (Symbol == IDENFR) {
            doOutPut();
            getsym();
            if (Symbol == ASSIGN) {
                doOutPut();
                getsym();
                if (Int()) {                    //整数，有预读
                    if (Symbol != COMMA) {      //为，则退出，保留预读
                        fileout << "<常量定义>" << endl;
                        return true;
                    } else {
                        while (Symbol == COMMA) { //不是，退出循环
                            doOutPut();
                            getsym();
                            if (Symbol == IDENFR) {
                                doOutPut();
                                getsym();
                                if (Symbol == ASSIGN) {
                                    doOutPut();
                                    getsym();
                                    if (!Int()) {
                                        error();
                                        return false;
                                    }
                                }
                            }
                        }
                        fileout << "<常量定义>" << endl;
                        return true;                //保留预读
                    }
                }
            }
        }
    } else if (Symbol == CHARTK) {
        doOutPut();
        getsym();
        if (Symbol == IDENFR) {
            doOutPut();
            getsym();
            if (Symbol == ASSIGN) {
                doOutPut();
                getsym();
                if (Symbol == CHARCON) {
                    doOutPut();
                    getsym();
                    if (Symbol != COMMA) {      //为，则退出，保留预读
                        fileout << "<常量定义>" << endl;
                        return true;
                    } else {
                        while (Symbol == COMMA) {
                            doOutPut();
                            getsym();
                            if (Symbol == IDENFR) {
                                doOutPut();
                                getsym();
                                if (Symbol == ASSIGN) {
                                    doOutPut();
                                    getsym();
                                    if (Symbol == CHARCON) {
                                        doOutPut();
                                        getsym();   //不是，退出循环
                                    }
                                }
                            }
                        }
                        fileout << "<常量定义>" << endl;
                        return true;                //保留预读
                    }
                }
            }
        }
    } else {
        error();
        return false;
    }
    return false;
}

//＜无符号整数＞  ::= ＜数字＞｛＜数字＞｝
bool unsignedInt() {
    if (Symbol == INTCON) {
        doOutPut();
        getsym();    //预读一个单词
        fileout << "<无符号整数>" << endl;
        return true;
    } else {
        return false;
    }
}

//＜整数＞        ::= ［＋｜－］＜无符号整数＞
bool Int() {
    if (Symbol == PLUS || Symbol == MINU) {
        doOutPut();
        getsym();
        if (unsignedInt()) {  //调用无符号整数
            fileout << "<整数>" << endl;
            return true;
        } else {
            return false;
        }
    } else {
        if (unsignedInt()) {  //直接调用无符号整数
            fileout << "<整数>" << endl;
            return true;
        } else {
            return false;
        }
    }
}

//＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞
bool deHead() {
    if (Symbol == INTTK || Symbol == CHARTK) {
        doOutPut();
        getsym();
        if (Symbol != IDENFR) {
            return false;
        }
        Symbology tmp;
        tmp.name.assign(token);
        tmp.flag = 1;
        symbology.push_back(tmp);
        doOutPut();
        fileout << "<声明头部>" << endl;
        getsym();  //预读
        return true;
    } else {
        return false;
    }
}

//＜常量＞   ::=  ＜整数＞|＜字符＞
bool Const() {
    if (Int()) {
        fileout << "<常量>" << endl;
        return true;
    } else if (Symbol == CHARCON) {
        doOutPut();
        fileout << "<常量>" << endl;
        getsym();  //预读
        return true;
    } else {
        return false;
    }
}

/*＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
 * int/char为起始符与有返回值函数声明first相同，尝试预读一个判断是否为(进行避免
 */
bool varDeclaration() {
    int tmpIndex;
    if (Symbol != INTTK && Symbol != CHARTK) {
        return false;
    }
    if (Symbol == INTTK || Symbol == CHARTK) {
        tmpIndex = indexs - token.length();
        getsym();
        if (Symbol == IDENFR) {
            getsym();
            if (Symbol == LPARENT) {        //读出为函数，则回退到int
                indexs = tmpIndex;
                getsym();
                return false;
            } else {
                indexs = tmpIndex;
                getsym();                   //进行预读，到int和char位置
                if (varDefinition()) {
                    if (Symbol != SEMICN) { //不是；报错
                        error();
                        return false;
                    }
                    doOutPut();
                    getsym();
                }
            }
        }
    }
    while (Symbol == INTTK || Symbol == CHARTK) {
        tmpIndex = indexs - token.length();
        getsym();
        if (Symbol == IDENFR) {
            getsym();
            if (Symbol == LPARENT) {        //读出为函数，则回退到int
                fileout << "<变量说明>" << endl;
                indexs = tmpIndex;
                getsym();                   //进行预读
                return true;
            } else {
                indexs = tmpIndex;
                getsym();                   //进行预读，到int和char位置
                if (varDefinition()) {
                    if (Symbol != SEMICN) { //不是；报错
                        error();
                        return false;
                    }
                    doOutPut();
                    getsym();
                }
            }
        }
    }
    fileout << "<变量说明>" << endl;
    return true;
}

//＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
bool varDefinition() {
    int tmpIndex = indexs;
    if (Symbol == INTTK)
        tmpIndex = indexs - 3;
    else if (Symbol == CHARTK)
        tmpIndex = indexs - 4;
    else
        return false;
    if (Symbol == INTTK || Symbol == CHARTK) {  //读到int/char
        getsym();
        if (Symbol == IDENFR) {
            getsym();
            if (Symbol == ASSIGN) {             //读到=判断出为初始化
                indexs = tmpIndex;
                getsym();
                if (varInitialDefinition()) {
                    fileout << "<变量定义>" << endl;
                    return true;
                }
            } else if (Symbol == LBRACK) {             //[
                getsym();
                if (Symbol == INTCON) {
                    getsym();
                    if (Symbol == RBRACK) {     //]
                        getsym();
                        if (Symbol == ASSIGN) {             //读到=判断出为初始化
                            indexs = tmpIndex;
                            getsym();
                            if (varInitialDefinition()) {
                                fileout << "<变量定义>" << endl;
                                return true;
                            }
                        } else if (Symbol == LBRACK) {             //[
                            getsym();
                            if (Symbol == INTCON) {
                                getsym();
                                if (Symbol == RBRACK) {     //]
                                    getsym();
                                    if (Symbol == ASSIGN) {             //读到=判断出为初始化
                                        indexs = tmpIndex;
                                        getsym();
                                        if (varInitialDefinition()) {
                                            fileout << "<变量定义>" << endl;
                                            return true;
                                        }
                                    } else {
                                        indexs = tmpIndex;
                                        getsym();
                                        if (varUnInitialDefinition()) {
                                            fileout << "<变量定义>" << endl;
                                            return true;
                                        }
                                    }
                                }
                            }
                        } else {                                        //读到其他，进入无初始化判断
                            indexs = tmpIndex;
                            getsym();
                            if (varUnInitialDefinition()) {
                                fileout << "<变量定义>" << endl;
                                return true;
                            }
                        }
                    }
                }
            } else {                                        //读到其他，进入无初始化判断
                indexs = tmpIndex;
                getsym();
                if (varUnInitialDefinition()) {
                    fileout << "<变量定义>" << endl;
                    return true;
                }
            }
        }

    }
    if (Symbol == SEMICN) {
        fileout << "<变量定义>" << endl;
        return true;
    }
    return false;
}

//＜变量定义无初始化＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'){,(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
bool varUnInitialDefinition() {
    if (Symbol != CHARTK && Symbol != INTTK) {
        error();
        return false;
    }
    doOutPut();
    getsym();
    if (Symbol != IDENFR) {
        error();
        return false;
    }
    while (Symbol == IDENFR) {                  //标志符
        doOutPut();
        getsym();
        if (Symbol == COMMA) {                  //,
            doOutPut();
            getsym();
            continue;
        } else if (Symbol == LBRACK) {              //[
            doOutPut();
            getsym();
            if (unsignedInt()) {                    //无符号
                if (Symbol == RBRACK) {             //]
                    doOutPut();
                    getsym();
                    //到达]判断后面为[还是，还是其他符号
                    if (Symbol == COMMA) {                  //,
                        doOutPut();
                        getsym();
                        continue;
                    } else if (Symbol == LBRACK) {          //[
                        doOutPut();
                        getsym();
                        if (unsignedInt()) {                //无符号
                            if (Symbol == RBRACK) {         //]
                                doOutPut();
                                getsym();
                            }
                            //到达]判断后面为，还是其他符号
                            if (Symbol == COMMA) {                  //,
                                doOutPut();
                                getsym();
                                continue;
                            } else {
                                fileout << "<变量定义无初始化>" << endl;
                                return true;                        //其他符号表示声明结束
                            }
                        }
                    } else {
                        fileout << "<变量定义无初始化>" << endl;
                        return true;                        //其他符号表示声明结束
                    }
                }
            }
        } else {
            fileout << "<变量定义无初始化>" << endl;
            return true;                        //其他符号表示声明结束
        }
    }
    if (Symbol == SEMICN) {
        fileout << "<变量定义无初始化>" << endl;
        return true;
    }
    return false;
}

//＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞|＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'|＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
bool varInitialDefinition() {
    if (Symbol != CHARTK && Symbol != INTTK) {
        return false;
    }
    doOutPut();
    getsym();
    if (Symbol == IDENFR) {
        doOutPut();
        getsym();
        if (Symbol == ASSIGN) {       //第一种情况
            doOutPut();
            getsym();
            if (Const()) {
                fileout << "<变量定义及初始化>" << endl;
                return true;
            }
        } else if (Symbol == LBRACK) {
            doOutPut();
            getsym();
            if (unsignedInt()) {
                if (Symbol == RBRACK) {
                    doOutPut();
                    getsym();
                    if (Symbol == ASSIGN) {   //第二种情况 ＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'
                        doOutPut();
                        getsym();
                        if (Symbol == LBRACE) {
                            doOutPut();
                            getsym();
                            if (!Const()) {
                                error();
                                return false;
                            }
                            while (Symbol == COMMA) {
                                doOutPut();
                                getsym();
                                if (!Const()) {
                                    error();
                                    return false;
                                }
                            }
                            if (Symbol == RBRACE) {
                                doOutPut();
                                getsym();
                                fileout << "<变量定义及初始化>" << endl;
                                return true;
                            }
                        }
                    } else if (Symbol == LBRACK) {   //'['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
                        doOutPut();
                        getsym();
                        if (unsignedInt()) {
                            if (Symbol == RBRACK) {
                                doOutPut();
                                getsym();
                                if (Symbol == ASSIGN) {
                                    doOutPut();
                                    getsym();
                                    if (Symbol == LBRACE) {   //'{'＜常量＞{,＜常量＞}'}'
                                        doOutPut();
                                        getsym();
                                        if (Symbol != LBRACE) {
                                            error();
                                            return false;
                                        }
                                        while (true) {
                                            doOutPut();
                                            if (Symbol != LBRACE) {
                                                break;
                                            }
                                            getsym();
                                            //'{'＜常量＞{,＜常量＞}'}'
                                            if (!Const()) {
                                                error();
                                                return false;
                                            }
                                            while (Symbol == COMMA) {
                                                doOutPut();
                                                getsym();
                                                if (!Const()) {
                                                    error();
                                                    return false;
                                                }
                                            }
                                            if (Symbol == RBRACE) {
                                                doOutPut();
                                                getsym();
                                            } else {
                                                error();
                                                return false;
                                            }
                                            if (Symbol == COMMA) {
                                                doOutPut();
                                                getsym();
                                            } else {
                                                break;
                                            }
                                        }
                                        if (Symbol == RBRACE) {
                                            doOutPut();
                                            getsym();
                                            fileout << "<变量定义及初始化>" << endl;
                                            return true;
                                        } else {
                                            error();
                                            return false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

//＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
bool valueReturnFunction() {
    if (deHead()) {
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (parameterTable()) {
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    if (Symbol == LBRACE) {
                        doOutPut();
                        getsym();
                        if (compoundStatement()) {
                            if (Symbol == RBRACE) {
                                doOutPut();
                                getsym();
                                fileout << "<有返回值函数定义>" << endl;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

//＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
bool noValueReturnFunction() {
    int tmpIndex = indexs - token.length();
    if (Symbol == VOIDTK) {
        getsym();
        if (Symbol == IDENFR) {
            indexs = tmpIndex;
            getsym();
            doOutPut();
            getsym();
            doOutPut();
            Symbology tmp;
            tmp.name.assign(token);
            tmp.flag = 0;
            symbology.push_back(tmp);
            getsym();
            if (Symbol == LPARENT) {
                doOutPut();
                getsym();
                if (parameterTable()) {
                    if (Symbol == RPARENT) {
                        doOutPut();
                        getsym();
                        if (Symbol == LBRACE) {
                            doOutPut();
                            getsym();
                            if (compoundStatement()) {
                                if (Symbol == RBRACE) {
                                    doOutPut();
                                    getsym();
                                    fileout << "<无返回值函数定义>" << endl;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        } else if (Symbol == MAINTK) {
            indexs = tmpIndex;
            getsym();
            return false;
        }
    }
    return false;
}

//＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
bool compoundStatement() {
    constDeclaration();
    varDeclaration();
    if (statementList()) {
        fileout << "<复合语句>" << endl;
        return true;
    } else {
        return false;
    }
}

//＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
bool parameterTable() {
    if (Symbol == RPARENT || Symbol == LBRACE) {  //) {
        fileout << "<参数表>" << endl;
        return true;
    }
    if (Symbol != CHARTK && Symbol != INTTK) {
        error();
        return false;
    } else {
        doOutPut();
        getsym();
        if (Symbol == IDENFR) {
            doOutPut();
            getsym();
            if (Symbol != COMMA) {
                fileout << "<参数表>" << endl;
                return true;
            }
            while (Symbol == COMMA) {
                doOutPut();
                getsym();
                if (Symbol != CHARTK && Symbol != INTTK) {
                    error();
                    return false;
                } else {
                    doOutPut();
                    getsym();
                    if (Symbol == IDENFR) {
                        doOutPut();
                        getsym();
                    }
                }
            }
            fileout << "<参数表>" << endl;
            return true;
        }
    }
    return false;
}

//＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
bool mainFunction() {
    if (Symbol == VOIDTK) {
        doOutPut();
        getsym();
        if (Symbol == MAINTK) {
            doOutPut();
            getsym();
            if (Symbol == LPARENT) {
                doOutPut();
                getsym();
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    if (Symbol == LBRACE) {
                        doOutPut();
                        getsym();
                        if (compoundStatement()) {
                            if (Symbol == RBRACE) {
                                doOutPut();
                                getsym();
                                fileout << "<主函数>" << endl;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

//＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}   //[+|-]只作用于第一个<项>
bool expression() {
    if (Symbol == PLUS) {
        doOutPut();
        getsym();
    } else if (Symbol == MINU) {
        doOutPut();
        getsym();
    }
    if (!item()) {
        error();
        return false;
    }
    do {
        if (Symbol == PLUS || Symbol == MINU) {
            doOutPut();
            getsym();
            continue;
        } else {
            fileout << "<表达式>" << endl;
            return true;
        }
    } while (item());
    fileout << "<表达式>" << endl;
    return true;
}

//＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
bool item() {
    if (factor()) {
        if (Symbol != MULT && Symbol != DIV) {
            fileout << "<项>" << endl;
            return true;
        }
        while (Symbol == MULT || Symbol == DIV) {
            doOutPut();
            getsym();
            if (!factor()) {
                error();
                return false;
            }
        }
        fileout << "<项>" << endl;
        return true;

    }
    return false;
}

//＜因子＞    ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'|＜标识符＞'['＜表达式＞']''['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
bool factor() {
    int tmpIndex = indexs - token.length();
    string nameTmp;
    if (Symbol == IDENFR) {  //当前是 ＜标识符＞｜＜标识符＞'['＜表达式＞']'|＜标识符＞'['＜表达式＞']''['＜表达式＞']' 也可能是 ＜有返回值函数调用语句＞
        nameTmp.assign(token);
        getsym();
        if (Symbol == LBRACK) {  //是[
            Symbol = IDENFR;
            token.assign(nameTmp);
            doOutPut();
            Symbol = LBRACK;
            token.assign("[");
            doOutPut();
            getsym();
            if (!expression()) {
                error();
                return false;
            }
            //表达式分析成功 并预读了一个单词
            if (Symbol != RBRACK) {
                error();
                return false;
            }
            if (Symbol == RBRACK) {  //是]
                doOutPut();
                getsym();   //为下一个预读 不管是啥
                if (Symbol != LBRACK) {
                    fileout << "<因子>" << endl;
                    return true;
                } else {            //[
                    doOutPut();
                    getsym();
                    if (!expression()) {
                        error();
                        return false;
                    }
                    if (Symbol == RBRACK) {
                        doOutPut();
                        getsym();
                        fileout << "<因子>" << endl;
                        return true;
                    }
                }
            } else {
                return false;
            }
        } else if (Symbol == LPARENT) {  //是( 说明这个有可能是  ＜有返回值函数调用语句＞  需要恢复
            indexs = tmpIndex;
            getsym();   //把标识符重新读出来
            //开始调用 有返回值的函数调用语句
            if (callValueReturnFunction()) {
                //调用有返回值的函数调用语句成功 并预读了一个单词
                fileout << "<因子>" << endl;
                return true;
            }
        } else {  //标识符后边不是[ 也不是(  对应文法＜标识符＞  直接返回 这个单词就是为下一个预读的单词了
            indexs = tmpIndex;
            getsym();   //把标识符重新读出来
            doOutPut();
            fileout << "<因子>" << endl;
            getsym();  //预读一个
            return true;
        }
    } else if (Symbol == LPARENT) {  // '('＜表达式＞')'
        doOutPut();
        getsym();
        //进入到对表达式的分析
        if (!expression()) {
            return false;
        }
        if (Symbol != RPARENT) {
            error();
            return false;
        }
        if (Symbol == RPARENT) {  //)
            doOutPut();
            getsym();   //为下一个预读 不管是啥
            fileout << "<因子>" << endl;
            return true;
        } else {
            return false;
        }
    } else if (Symbol == CHARCON) {  //当前是字符 对应文法 ＜字符＞
        doOutPut();
        getsym();   //预读
        fileout << "<因子>" << endl;
        return true;
    } else if (Int()) {  //整数
        fileout << "<因子>" << endl;
        return true;
    } else {
        error();
        return false;
    }
    return false;
}

//＜语句＞    ::= ＜循环语句＞｜＜条件语句＞| ＜有返回值函数调用语句＞;  |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜情况语句＞｜＜空＞;|＜返回语句＞; | '{'＜语句列＞'}'          /*测试程序需出现11种语句*/
//语法分析，使用vector容器充当"符号表"记录函数名和有无返回值
bool Statement() {
    if (Symbol == SEMICN) {  //;分号  一个分号直接就是一个语句
        doOutPut();
        fileout << "<语句>" << endl;
        getsym();  //预读一个 不管是啥
        return true;
    } else if (Symbol == RETURNTK) {  //＜返回语句＞;
        if (returnStatement()) {
            //分析返回语句成功 并预读了一个符号
            if (Symbol != SEMICN) {
                error();
                return false;
            }
            if (Symbol == SEMICN) {  //;
                doOutPut();
                fileout << "<语句>" << endl;
                getsym();  //预读
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else if (Symbol == SCANFTK) {  //＜读语句＞;
        if (readStatement()) {
            //分析读语句成功 并预读了一个符号
            if (Symbol != SEMICN) {
                error();
                return false;
            }
            if (Symbol == SEMICN) {  //;
                doOutPut();
                fileout << "<语句>" << endl;
                getsym();  //预读
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else if (Symbol == PRINTFTK) {  //＜写语句＞;
        if (writeStatement()) {
            //分析写语句成功 并预读了一个符号
            if (Symbol != SEMICN) {
                error();
                return false;
            }
            if (Symbol == SEMICN) {  //;
                doOutPut();
                fileout << "<语句>" << endl;
                getsym();  //预读
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else if (Symbol == IFTK) {  //＜条件语句＞
        if (conditionStatement()) {
            //分析条件语句成功 并预读了一个符号
            fileout << "<语句>" << endl;
            return true;
        } else {
            return false;
        }
    } else if (Symbol == WHILETK || Symbol == FORTK) {  //＜循环语句＞
        if (loopStatement()) {
            fileout << "<语句>" << endl;
            return true;
        } else {
            return false;
        }
    } else if (Symbol == SWITCHTK) {  //＜情况语句＞;
        if (switchStatement()) {
            //分析读语句成功 并预读了一个符号
            fileout << "<语句>" << endl;
            return true;
        } else {
            return false;
        }
    } else if (Symbol == LBRACE) {  //'{'＜语句列＞'}'
        doOutPut();
        getsym();
        if (!statementList()) {  //分析语句列
            return false;
        }
        if (Symbol == RBRACE) {  //}
            doOutPut();
            fileout << "<语句>" << endl;
            getsym();  //预读一个 不管是啥
            return true;
        } else {
            return false;
        }
    } else if (Symbol == IDENFR) {  //＜有返回值函数调用语句＞; |＜无返回值函数调用语句＞;｜＜赋值语句＞;
        int tmpIndex = indexs - token.length();
        getsym();
        if (Symbol == LBRACK || Symbol == ASSIGN) {  //[和= 说明是赋值语句
            indexs = tmpIndex;
            getsym();
            if (!assignStatement()) {
                return false;
            }
            //分析赋值语句成功 并预读了一个单词
            if (Symbol != SEMICN) {
                error();
                return false;
            }
            if (Symbol == SEMICN) {  //;分号
                doOutPut();
                fileout << "<语句>" << endl;
                getsym();
                return true;
            } else {
                return false;
            }
        } else if (Symbol == LPARENT) {  //( 说明是函数调用语句
            indexs = tmpIndex;             //回退到读标志符前
            getsym();
            if (!isValueReturn()) {
                if (!callNoValueReturnFunction()) {
                    return false;
                }
                if (Symbol != SEMICN) {
                    error();
                    return false;
                }
                if (Symbol == SEMICN) {  //;
                    doOutPut();
                    fileout << "<语句>" << endl;
                    getsym();
                    return true;
                } else {
                    return false;
                }
            } else {
                if (!callValueReturnFunction()) {
                    return false;
                }
                if (Symbol != SEMICN) {
                    error();
                    return false;
                }
                if (Symbol == SEMICN) {  //;
                    doOutPut();
                    fileout << "<语句>" << endl;
                    getsym();
                    return true;
                } else {
                    return false;
                }
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

//＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞|＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
bool assignStatement() {
    if (Symbol == IDENFR) {
        doOutPut();
        getsym();
        if (Symbol == ASSIGN) {       //第一种情况
            doOutPut();
            getsym();
            if (expression()) {
                fileout << "<赋值语句>" << endl;
                return true;
            }
        } else if (Symbol == LBRACK) {
            doOutPut();
            getsym();
            if (expression()) {
                if (Symbol == RBRACK) {
                    doOutPut();
                    getsym();
                    if (Symbol == ASSIGN) {   //第二种情况
                        doOutPut();
                        getsym();
                        if (expression()) {
                            fileout << "<赋值语句>" << endl;
                            return true;
                        }
                    } else if (Symbol == LBRACK) {
                        doOutPut();
                        getsym();
                        if (expression()) {
                            if (Symbol == RBRACK) {
                                doOutPut();
                                getsym();
                                if (Symbol == ASSIGN) {   //第三种情况
                                    doOutPut();
                                    getsym();
                                    if (expression()) {
                                        fileout << "<赋值语句>" << endl;
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

//＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
bool conditionStatement() {
    if (Symbol == IFTK) {  //if
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {  //(
            doOutPut();
            getsym();
            if (!condition()) {  //分析条件
                return false;
            }
            if (Symbol == RPARENT) {  //)
                doOutPut();
                getsym();
                if (!Statement()) {  //分析语句
                    return false;
                }
                //开始分析［else＜语句＞］
                if (Symbol == ELSETK) {  //else
                    doOutPut();
                    getsym();
                    if (!Statement()) {  //分析语句
                        return false;
                    }
                }
                fileout << "<条件语句>" << endl;
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

//＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
bool condition() {
    if (expression()) {
        if (Relation()) {
            if (expression()) {
                fileout << "<条件>" << endl;
                return true;
            }
        }
    }
    return false;
}

//＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞| for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
bool loopStatement() {
    if (Symbol == WHILETK) {    //while
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (condition()) {
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    if (Statement()) {
                        fileout << "<循环语句>" << endl;
                        return true;
                    }
                }
            }
        }
    } else if (Symbol == FORTK) {    //for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (Symbol == IDENFR) {
                doOutPut();
                getsym();
                if (Symbol == ASSIGN) {
                    doOutPut();
                    getsym();
                    if (expression()) {
                        if (Symbol == SEMICN) {
                            doOutPut();
                            getsym();
                            if (condition()) {
                                if (Symbol == SEMICN) {
                                    doOutPut();
                                    getsym();
                                    if (Symbol == IDENFR) {
                                        doOutPut();
                                        getsym();
                                        if (Symbol == ASSIGN) {
                                            doOutPut();
                                            getsym();
                                            if (Symbol == IDENFR) {
                                                doOutPut();
                                                getsym();
                                                if (Symbol == PLUS || Symbol == MINU) {
                                                    doOutPut();
                                                    getsym();
                                                    if (step()) {
                                                        if (Symbol == RPARENT) {
                                                            doOutPut();
                                                            getsym();
                                                            if (Statement()) {
                                                                fileout << "<循环语句>" << endl;
                                                                return true;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

//＜步长＞::= ＜无符号整数＞
bool step() {
    if (unsignedInt()) {
        fileout << "<步长>" << endl;
        return true;
    } else {
        error();
        return false;
    }
}

//＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
bool switchStatement() {
    if (Symbol == SWITCHTK) {
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (expression()) {
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    if (Symbol == LBRACE) {
                        doOutPut();
                        getsym();
                        if (caseTable()) {
                            if (defaultStatement()) {
                                if (Symbol == RBRACE) {
                                    doOutPut();
                                    getsym();
                                    fileout << "<情况语句>" << endl;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

//＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
bool caseTable() {
    if (!caseStatement()) {
        error();
        return false;
    } else {
        while (caseStatement()) {
        }
        fileout << "<情况表>" << endl;
        return true;
    }
}

//＜情况子语句＞  ::=  case＜常量＞：＜语句＞
bool caseStatement() {
    if (Symbol == CASETK) {
        doOutPut();
        getsym();
        if (Const()) {
            if (Symbol == COLON) {
                doOutPut();
                getsym();
                if (Statement()) {
                    fileout << "<情况子语句>" << endl;
                    return true;
                }
            }
        }
    }
    return false;
}

//＜缺省＞   ::=  default :＜语句＞
bool defaultStatement() {
    if (Symbol == DEFAULTTK) {
        doOutPut();
        getsym();
        if (Symbol == COLON) {
            doOutPut();
            getsym();
            if (Statement()) {
                fileout << "<缺省>" << endl;
                return true;
            }
        }
    }
    return false;
}

//＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
bool callValueReturnFunction() {
    if (Symbol == IDENFR) {
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (valueParameterTable()) {
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    fileout << "<有返回值函数调用语句>" << endl;
                    return true;
                }
            }
        }
    }
    return false;
}

//＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
bool callNoValueReturnFunction() {
    if (Symbol == IDENFR) {
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (valueParameterTable()) {
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    fileout << "<无返回值函数调用语句>" << endl;
                    return true;
                }
            }
        }
    }
    return false;
}

//＜值参数表＞  ::= ＜表达式＞{,＜表达式＞}｜＜空＞
bool valueParameterTable() {
    if (Symbol == RPARENT) {                //如果直接为右括号，则为空
        fileout << "<值参数表>" << endl;
        return true;
    } else if (expression()) {
        if (Symbol != COMMA) {              //不是逗号退出循环
            fileout << "<值参数表>" << endl;
            return true;
        } else {
            doOutPut();
            getsym();
            if (!expression()) {
                error();
                return false;
            }
            do {          //表达式和，循环
                if (Symbol != COMMA) {
                    fileout << "<值参数表>" << endl;
                    return true;
                }
                doOutPut();
                getsym();
            } while (expression());
        }
    }
    return false;
}

//＜语句列＞   ::= ｛＜语句＞｝
bool statementList() {
    while (Symbol != RBRACE) { //到右括号停止
        if (!Statement()) {  //不是语句,退出
            break;
        }
    }
    fileout << "<语句列>" << endl;
    return true;
}

//＜读语句＞    ::=  scanf '('＜标识符＞{,＜标识符＞}')’
bool readStatement() {
    if (Symbol == SCANFTK) {        //scanf
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {    //(
            doOutPut();
            getsym();
            if (Symbol == IDENFR) { //标识符
                doOutPut();
                getsym();
                if (Symbol == COMMA) {   //,
                    doOutPut();
                    getsym();
                    while (Symbol == IDENFR) { //{标识符}
                        doOutPut();
                        getsym();
                        if (Symbol != COMMA) {
                            doOutPut();
                            getsym();
                            break;
                        }
                        doOutPut();
                        getsym();
                    }
                    if (Symbol == RPARENT) {    //）
                        doOutPut();
                        getsym();
                        fileout << "<读语句>" << endl;
                        return true;
                    } else {
                        error();
                        return false;
                    }
                } else if (Symbol == RPARENT) {    //）
                    doOutPut();
                    getsym();
                    fileout << "<读语句>" << endl;
                    return true;
                } else {
                    error();
                    return false;
                }
            }
        }
    }
    return false;
}

//＜写语句＞    ::= printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')’
bool writeStatement() {
    if (Symbol == PRINTFTK) {                           //printf
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {                        //(
            doOutPut();
            getsym();
            if (String()) {                         //字符串
                if (Symbol == COMMA) {                          //,
                    doOutPut();
                    getsym();
                    if (expression()) {                          //表达式
                        if (Symbol == RPARENT) {                //）
                            doOutPut();
                            getsym();
                            fileout << "<写语句>" << endl;
                            return true;
                        } else {
                            error();
                            return false;
                        }
                    } else {
                        error();
                        return false;
                    }
                } else if (Symbol == RPARENT) {         //）
                    doOutPut();
                    getsym();
                    fileout << "<写语句>" << endl;
                    return true;
                }
            } else if (expression()) {                       //表达式
                if (Symbol == RPARENT) {                    //）
                    doOutPut();
                    getsym();
                    fileout << "<写语句>" << endl;
                    return true;
                } else {
                    error();
                    return false;
                }
            } else {
                error();
                return false;
            }
        } else {
            error();
            return false;
        }
    } else {
        error();
        return false;
    }
    return false;
}

//＜返回语句＞   ::=  return['('＜表达式＞')']
bool returnStatement() {
    if (Symbol == RETURNTK) {
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (expression()) {
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    fileout << "<返回语句>" << endl;
                    return true;
                } else {
                    error();
                    return false;
                }
            } else {
                error();
                return false;
            }
        } else if (Symbol == SEMICN) {
            fileout << "<返回语句>" << endl;
            return true;
        } else {
            error();
            return false;
        }
    } else {
        error();
        return false;
    }
}

//关系运算符
bool Relation() {
    if (Symbol == LSS || Symbol == LEQ || Symbol == GRE || Symbol == GEQ || Symbol == EQL || Symbol == NEQ) {
        doOutPut();
        getsym();
        return true;
    } else {
        return false;
    }
}

//查找vecter中该函数是否有返回值
bool isValueReturn() {
    for (Symbology s: symbology) {
        if (!s.name.compare(token)) {
            return s.flag;
        }
    }
    std::cout << "未找到该函数" << std::endl;
    return -1;
}
