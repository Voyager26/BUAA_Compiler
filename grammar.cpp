//
// Created by Phantom on 2020/10/10.
//
#include <iostream>
#include <string>
#include "grammar.h"
#include "lexical.h"
#include "globalVar.h"

using namespace std;
string funName;
string nowConst;
string localName;
int localType;
int labelMark = 0;
int hasReturn = 0;
int localAddr = 3;  //0-ra,1-fp,2-sp,3...局部变量
int globalAddr = 0;
int tmpVarId = 0;  //中间变量的id
vector<CASEInfo> caseInfo;

//＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
bool String(string &tmp) {
    if (Symbol == STRCON) {
        tmp.assign(token);
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
    constDeclaration(1);
    varDeclaration(1);
    midCodeTable.push_back(midCode(GoTo, "main", "", ""));
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
bool constDeclaration(int global) {
    if (Symbol != CONSTTK) {
        return false;
    }
    if (Symbol == CONSTTK) {
        doOutPut();
        getsym();
        if (constDefinition(global)) {     //是否为常量定义，预读了下一个符号
            if (Symbol != SEMICN) { //不是;报错
                retractG();
                fileerror << lines << " k\n";
                Symbol = SEMICN;
            }
            if (Symbol == SEMICN) {
                doOutPut();
                getsym();
                while (Symbol == CONSTTK) {     //不是const则常量说明结束
                    doOutPut();
                    getsym();
                    if (!constDefinition(global)) {
                        error();
                        return false;
                    }
                    if (Symbol != SEMICN) {
                        retractG();
                        fileerror << lines << " k\n";
                        Symbol = SEMICN;
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
bool constDefinition(int global) {
    if (Symbol == INTTK) {
        doOutPut();
        getsym();
        if (Symbol == IDENFR) {
            string name = inLowerToken;
            doOutPut();
            getsym();
            if (Symbol == ASSIGN) {
                doOutPut();
                getsym();
                if (!Int()) {                    //整数，有预读
                    fileerror << lines << " o\n";
                    getsym();
                }
                int tmpNum = num;
                doOutPut();
                if (global) {
                    if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                        globalSymbolMap.insert(make_pair(name, Symbology(name, "const", "int", num, globalAddr)));
                        globalAddr++;
                    } else {  //找到了 说明重定义了
                        fileerror << lines << " b\n";
                    }
                } else {
                    if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                        localSymbolMap.insert(make_pair(name, Symbology(name, "const", "int", num, localAddr)));
                        localAddr++;
                    } else {  //找到了 说明重定义了
                        fileerror << lines << " b\n";
                    }
                }
                midCodeTable.push_back(midCode(Con, name, "int", toString(tmpNum)));
                if (Symbol != COMMA) {      //为，则退出，保留预读
                    fileout << "<常量定义>" << endl;
                    return true;
                } else {
                    while (Symbol == COMMA) { //不是，退出循环
                        doOutPut();
                        getsym();
                        if (Symbol == IDENFR) {
                            name.assign(inLowerToken);
                            doOutPut();
                            getsym();
                            if (Symbol == ASSIGN) {
                                doOutPut();
                                getsym();
                                if (!Int()) {                    //整数，有预读
                                    fileerror << lines << " o\n";
                                    getsym();
                                }
                                tmpNum = num;
                                doOutPut();
                                if (global) {
                                    if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                                        globalSymbolMap.insert(
                                                make_pair(name, Symbology(name, "const", "int", num, globalAddr)));
                                        globalAddr++;
                                    } else {  //找到了 说明重定义了
                                        fileerror << lines << " b\n";
                                    }
                                } else {
                                    if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                                        localSymbolMap.insert(
                                                make_pair(name, Symbology(name, "const", "int", num, localAddr)));
                                        localAddr++;
                                    } else {  //找到了 说明重定义了
                                        fileerror << lines << " b\n";
                                    }
                                }
                                midCodeTable.push_back(midCode(Con, name, "int", toString(tmpNum)));
                            }
                        }
                    }
                    fileout << "<常量定义>" << endl;
                    return true;                //保留预读
                }
            }
        }
    } else if (Symbol == CHARTK) {
        doOutPut();
        getsym();
        if (Symbol == IDENFR) {
            string name;
            name.assign(inLowerToken);
            doOutPut();
            getsym();
            if (Symbol == ASSIGN) {
                doOutPut();
                getsym();
                if (Symbol != CHARCON) {
                    fileerror << lines << " o\n";
                }
                int tmpV = (int) token[0];
                if (global) {
                    if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                        globalSymbolMap.insert(make_pair(name, Symbology(name, "const", "char", tmpV, globalAddr)));
                        globalAddr++;
                    } else {  //找到了 说明重定义了
                        fileerror << lines << " b\n";
                    }
                } else {
                    if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                        localSymbolMap.insert(make_pair(name, Symbology(name, "const", "char", tmpV, localAddr)));
                        localAddr++;
                    } else {  //找到了 说明重定义了
                        fileerror << lines << " b\n";
                    }
                }
                midCodeTable.push_back(midCode(Con, name, "char", toString((char) tmpV)));
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
                            name.assign(inLowerToken);
                            doOutPut();
                            getsym();
                            if (Symbol == ASSIGN) {
                                doOutPut();
                                getsym();
                                if (Symbol != CHARCON) {
                                    fileerror << lines << " o\n";
                                }
                                int tmpV = (int) token[0];
                                if (global) {
                                    if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                                        globalSymbolMap.insert(
                                                make_pair(name, Symbology(name, "const", "char", tmpV, globalAddr)));
                                        globalAddr++;
                                    } else {  //找到了 说明重定义了
                                        fileerror << lines << " b\n";
                                    }
                                } else {
                                    if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                                        localSymbolMap.insert(
                                                make_pair(name, Symbology(name, "const", "char", tmpV, localAddr)));
                                        localAddr++;
                                    } else {  //找到了 说明重定义了
                                        fileerror << lines << " b\n";
                                    }
                                }
                                midCodeTable.push_back(midCode(Con, name, "char", toString((char) tmpV)));
                                doOutPut();
                                getsym();
                            }
                        }
                    }
                    fileout << "<常量定义>" << endl;
                    return true;                //保留预读
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
    bool isMinu;
    if (Symbol == PLUS || Symbol == MINU) {
        isMinu = (Symbol == MINU);
        doOutPut();
        getsym();
        if (unsignedInt()) {  //调用无符号整数
            if (isMinu) {
                num = -1 * num;
            }
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
bool deHead(int *t) {
    if (Symbol == INTTK || Symbol == CHARTK) {
        string type;
        type.assign(inLowerToken);
        if (type.compare("int") == 0) {
            *t = 0;
        } else {
            *t = 1;
        }
        doOutPut();
        getsym();
        if (Symbol != IDENFR) {
            return false;
        }
        funName.assign(inLowerToken);
        if (globalSymbolMap.find(funName) == globalSymbolMap.end()) {  //没找到
            globalSymbolMap.insert(make_pair(funName, Symbology(funName, "fun", type)));
        } else {
            fileerror << lines << " b\n";
        }
        midCodeTable.push_back(midCode(FuncOp, type, funName, ""));
        doOutPut();
        fileout << "<声明头部>" << endl;
        getsym();  //预读
        return true;
    } else {
        return false;
    }
}

//＜常量＞   ::=  ＜整数＞|＜字符＞
bool Const(int *t) {
    if (Int()) {
        *t = 0;
        nowConst.assign(toString(num));
        fileout << "<常量>" << endl;
        return true;
    } else if (Symbol == CHARCON) {
        doOutPut();
        *t = 1;
        fileout << "<常量>" << endl;
        int tmp = (int)con_ch;
        nowConst.assign(toString(con_ch));
        getsym();  //预读
        return true;
    }
    error();
    return false;
}

/*＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
 * int/char为起始符与有返回值函数声明first相同，尝试预读一个判断是否为(进行避免
 */
bool varDeclaration(int global) {
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
                retractG(tmpIndex);
                getsym();
                return false;
            } else {
                retractG(tmpIndex);
                getsym();                   //进行预读，到int和char位置
                if (varDefinition(global)) {
                    if (Symbol != SEMICN) { //不是；报错
                        retractG();
                        fileerror << lines << " k\n";
                        Symbol = SEMICN;
                    }
                    if (Symbol == SEMICN) {
                        doOutPut();
                        getsym();
                    }
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
                retractG(tmpIndex);
                getsym();                   //进行预读
                return true;
            } else {
                retractG(tmpIndex);
                getsym();                   //进行预读，到int和char位置
                if (varDefinition(global)) {
                    if (Symbol != SEMICN) { //不是；报错
                        retractG();
                        fileerror << lines << " k\n";
                        Symbol = SEMICN;
                    }
                    if (Symbol == SEMICN) {  //;分号
                        doOutPut();
                        getsym();
                    }
                }
            }
        }
    }
    fileout << "<变量说明>" << endl;
    return true;
}

//＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
bool varDefinition(int global) {
    int oldindex = indexs - token.length();
    if (Symbol == INTTK || Symbol == CHARTK) {  //读到int/char
        getsym();
        if (Symbol == IDENFR) {
            getsym();
            if (Symbol == ASSIGN) {             //读到=判断出为初始化
                retractG(oldindex);
                getsym();
                if (varInitialDefinition(global)) {
                    fileout << "<变量定义>" << endl;
                    return true;
                }
            } else if (Symbol == LBRACK) {             //[
                getsym();
                if (Symbol == INTCON) {

                }
                getsym();
                if (Symbol != RBRACK) { //不是]报错
                    retractG();
                    Symbol = RBRACK;
                }
                if (Symbol == RBRACK) {     //]
                    getsym();
                    if (Symbol == ASSIGN) {             //读到=判断出为初始化
                        retractG(oldindex);
                        getsym();
                        if (varInitialDefinition(global)) {
                            fileout << "<变量定义>" << endl;
                            return true;
                        }
                    } else if (Symbol == LBRACK) {             //[
                        getsym();
                        if (Symbol == INTCON) {
                        }
                        getsym();
                        if (Symbol != RBRACK) { //不是]报错
                            retractG();
                            Symbol = RBRACK;
                        }
                        if (Symbol == RBRACK) {     //]
                            getsym();
                            if (Symbol == ASSIGN) {             //读到=判断出为初始化
                                retractG(oldindex);
                                getsym();
                                if (varInitialDefinition(global)) {
                                    fileout << "<变量定义>" << endl;
                                    return true;
                                }
                            } else {
                                retractG(oldindex);
                                getsym();
                                if (varUnInitialDefinition(global)) {
                                    fileout << "<变量定义>" << endl;
                                    return true;
                                }
                            }
                        }
                    } else {                                        //读到其他，进入无初始化判断
                        retractG(oldindex);
                        getsym();
                        if (varUnInitialDefinition(global)) {
                            fileout << "<变量定义>" << endl;
                            return true;
                        }
                    }
                }
            } else {                                        //读到其他，进入无初始化判断
                retractG(oldindex);
                getsym();
                if (varUnInitialDefinition(global)) {
                    fileout << "<变量定义>" << endl;
                    return true;
                }
            }
        }

    }
    if (Symbol != SEMICN) { //不是;报错
        retractG();
        fileerror << lines << " m\n";
        Symbol = SEMICN;
    }
    if (Symbol == SEMICN) {
        fileout << "<变量定义>" << endl;
        return true;
    }
    return false;
}

//＜变量定义无初始化＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'){,(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
bool varUnInitialDefinition(int global) {
    vector<int> arrayInfo;
    if (Symbol != CHARTK && Symbol != INTTK) {
        error();
        return false;
    }
    string type = inLowerToken;
    doOutPut();
    getsym();
    if (Symbol != IDENFR) {
        error();
        return false;
    }
    while (Symbol == IDENFR) {                  //标志符
        arrayInfo.clear();
        string name = inLowerToken;
        doOutPut();
        getsym();
        if (Symbol == COMMA) {                  //,
            if (global) {
                if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                    globalSymbolMap.insert(make_pair(name, Symbology(name, "var", type, 0, globalAddr)));
                    globalAddr++;
                } else {  //找到了 说明重定义了
                    fileerror << lines << " b\n";
                }
            } else {
                if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                    localSymbolMap.insert(make_pair(name, Symbology(name, "var", type, 0, localAddr)));
                    localAddr++;
                } else {  //找到了 说明重定义了
                    fileerror << lines << " b\n";
                }
            }
            midCodeTable.push_back(midCode(VarOp, name, type, ""));
            doOutPut();
            getsym();
            continue;
        } else if (Symbol == LBRACK) {              //[
            doOutPut();
            getsym();
            if (!unsignedInt()) {                    //无符号
                fileerror << lines << " i\n";
                getsym();
            }
            int tmpArr1 = num;                  //一维个数
            arrayInfo.push_back(num);
            if (Symbol != RBRACK) { //不是]报错
                retractG();
                fileerror << lines << " m\n";
                Symbol = RBRACK;
            }
            if (Symbol == RBRACK) {             //]
                doOutPut();
                getsym();
                //到达]判断后面为[还是，还是其他符号
                if (Symbol == COMMA) {                  //,
                    arrayInfo.insert(arrayInfo.begin(), 1);
                    if (global) {
                        if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                            globalSymbolMap.insert(
                                    make_pair(name, Symbology(name, "array", type, arrayInfo, globalAddr)));
                            globalAddr += tmpArr1;
                        } else {  //找到了 说明重定义了
                            fileerror << lines << " b\n";
                        }
                    } else {
                        if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                            localSymbolMap.insert(
                                    make_pair(name, Symbology(name, "array", type, arrayInfo, localAddr)));
                            localAddr += tmpArr1;
                        } else {  //找到了 说明重定义了
                            fileerror << lines << " b\n";
                        }
                    }
                    midCodeTable.push_back(midCode(ArrayOp, name, type, "0"));
                    (midCodeTable.end() - 1)->insert(1);
                    (midCodeTable.end() - 1)->insert(tmpArr1);
                    doOutPut();
                    getsym();
                    continue;
                } else if (Symbol == LBRACK) {          //[
                    doOutPut();
                    getsym();
                    if (!unsignedInt()) {                    //无符号
                        fileerror << lines << " i\n";
                        getsym();
                    }
                    int tmpArr2 = num;
                    arrayInfo.push_back(num);
                    if (Symbol != RBRACK) { //不是]报错
                        retractG();
                        fileerror << lines << " m\n";
                        Symbol = RBRACK;
                    }
                    if (Symbol == RBRACK) {         //]
                        doOutPut();
                        getsym();
                    }
                    //到达]判断后面为，还是其他符号
                    if (Symbol == COMMA) {                  //,
                        arrayInfo.insert(arrayInfo.begin(), 2);
                        if (global) {
                            if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                                globalSymbolMap.insert(
                                        make_pair(name, Symbology(name, "array", type, arrayInfo, globalAddr)));
                                globalAddr += tmpArr1 * tmpArr2;
                            } else {  //找到了 说明重定义了
                                fileerror << lines << " b\n";
                            }
                        } else {
                            if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                                localSymbolMap.insert(
                                        make_pair(name, Symbology(name, "array", type, arrayInfo, localAddr)));
                                localAddr += tmpArr1 * tmpArr2;
                            } else {  //找到了 说明重定义了
                                fileerror << lines << " b\n";
                            }
                        }
                        midCodeTable.push_back(midCode(ArrayOp, name, type, "0"));
                        (midCodeTable.end() - 1)->insert(2);
                        (midCodeTable.end() - 1)->insert(tmpArr1);
                        (midCodeTable.end() - 1)->insert(tmpArr2);
                        doOutPut();
                        getsym();
                        continue;
                    } else {
                        arrayInfo.insert(arrayInfo.begin(), 2);
                        if (global) {
                            if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                                globalSymbolMap.insert(
                                        make_pair(name, Symbology(name, "array", type, arrayInfo, globalAddr)));
                                globalAddr += tmpArr1 * tmpArr2;
                            } else {  //找到了 说明重定义了
                                fileerror << lines << " b\n";
                            }
                        } else {
                            if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                                localSymbolMap.insert(
                                        make_pair(name, Symbology(name, "array", type, arrayInfo, localAddr)));
                                localAddr += tmpArr1 * tmpArr2;
                            } else {  //找到了 说明重定义了
                                fileerror << lines << " b\n";
                            }
                        }
                        midCodeTable.push_back(midCode(ArrayOp, name, type, "0"));
                        (midCodeTable.end() - 1)->insert(2);
                        (midCodeTable.end() - 1)->insert(tmpArr1);
                        (midCodeTable.end() - 1)->insert(tmpArr2);
                        fileout << "<变量定义无初始化>" << endl;
                        return true;                        //其他符号表示声明结束
                    }
                } else {
                    arrayInfo.insert(arrayInfo.begin(), 1);
                    if (global) {
                        if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                            globalSymbolMap.insert(
                                    make_pair(name, Symbology(name, "array", type, arrayInfo, globalAddr)));
                            globalAddr += tmpArr1;
                        } else {  //找到了 说明重定义了
                            fileerror << lines << " b\n";
                        }
                    } else {
                        if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                            localSymbolMap.insert(
                                    make_pair(name, Symbology(name, "array", type, arrayInfo, localAddr)));
                            localAddr += tmpArr1;
                        } else {  //找到了 说明重定义了
                            fileerror << lines << " b\n";
                        }
                    }
                    midCodeTable.push_back(midCode(ArrayOp, name, type, "0"));
                    (midCodeTable.end() - 1)->insert(1);
                    (midCodeTable.end() - 1)->insert(tmpArr1);
                    fileout << "<变量定义无初始化>" << endl;
                    return true;                        //其他符号表示声明结束
                }
            }
        } else {
            if (global) {
                if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                    globalSymbolMap.insert(make_pair(name, Symbology(name, "var", type, 0, globalAddr)));
                    globalAddr++;
                } else {  //找到了 说明重定义了
                    fileerror << lines << " b\n";
                }
            } else {
                if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                    localSymbolMap.insert(make_pair(name, Symbology(name, "var", type, 0, localAddr)));
                    localAddr++;
                } else {  //找到了 说明重定义了
                    fileerror << lines << " b\n";
                }
            }
            midCodeTable.push_back(midCode(VarOp, name, type, ""));
            fileout << "<变量定义无初始化>" << endl;
            return true;                        //其他符号表示声明结束
        }
    }
    if (Symbol != SEMICN) { //不是;报错
        retractG();
        fileerror << lines << " k\n";
        Symbol = SEMICN;
    }
    if (Symbol == SEMICN) {
        fileout << "<变量定义无初始化>" << endl;
        return true;
    }
    return false;
}

//＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞|＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'|＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
bool varInitialDefinition(int global) {
    vector<int> arrayInfo;
    if (Symbol != CHARTK && Symbol != INTTK) {
        return false;
    }
    int constType, compareType;
    string type = inLowerToken;
    if (type.compare("int") == 0) {
        compareType = 0;
    } else {
        compareType = 1;
    }
    doOutPut();
    getsym();
    if (Symbol == IDENFR) {
        string name = inLowerToken;
        doOutPut();
        getsym();
        if (Symbol == ASSIGN) {       //第一种情况
            doOutPut();
            getsym();
            if (Const(&constType)) {
                if (constType != compareType) {
                    fileerror << lines << " o\n";
                }
                if (global) {
                    if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                        globalSymbolMap.insert(
                                make_pair(name, Symbology(name, "var", type, toInt(nowConst), globalAddr)));
                        globalAddr++;
                    } else {  //找到了 说明重定义了
                        fileerror << lines << " b\n";
                    }
                } else {
                    if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                        localSymbolMap.insert(
                                make_pair(name, Symbology(name, "var", type, toInt(nowConst), localAddr)));
                        localAddr++;
                    } else {  //找到了 说明重定义了
                        fileerror << lines << " b\n";
                    }
                }
                midCodeTable.push_back(midCode(VvarOp, name, type, nowConst));
                fileout << "<变量定义及初始化>" << endl;
                return true;
            }
        } else if (Symbol == LBRACK) {
            doOutPut();
            getsym();
            if (!unsignedInt()) {                    //无符号
                fileerror << lines << " i\n";
                getsym();
            }
            int num1 = num;
            arrayInfo.push_back(num);
            if (Symbol != RBRACK) { //不是]报错
                retractG();
                fileerror << lines << " m\n";
                Symbol = RBRACK;
            }
            if (Symbol == RBRACK) {
                doOutPut();
                getsym();
                if (Symbol == ASSIGN) {   //第二种情况 ＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'
                    doOutPut();
                    getsym();
                    if (Symbol == LBRACE) {
                        doOutPut();
                        getsym();
                        if (!Const(&constType)) {
                            error();
                            return false;
                        }
                        if (constType != compareType) {
                            fileerror << lines << " o\n";
                        }
                        int i = 1;
                        midCodeTable.push_back(midCode(ArrayOp, name, type, "1"));
                        (midCodeTable.end() - 1)->insert(1);
                        (midCodeTable.end() - 1)->insert(num1);
                        (midCodeTable.end() - 1)->insert(toInt(nowConst));
                        while (Symbol == COMMA) {
                            i++;
                            doOutPut();
                            getsym();
                            if (!Const(&constType)) {
                                error();
                                return false;
                            }
                            if (constType != compareType) {
                                fileerror << lines << " o\n";
                            }
                            (midCodeTable.end() - 1)->insert(toInt(nowConst));
                        }
                        if (Symbol == RBRACE) {
                            if (i != num1) {
                                fileerror << lines << " n\n";
                            }
                            arrayInfo.insert(arrayInfo.begin(), 1);
                            if (global) {
                                if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                                    globalSymbolMap.insert(
                                            make_pair(name, Symbology(name, "array", type, arrayInfo, globalAddr)));
                                    globalAddr += num1;
                                } else {  //找到了 说明重定义了
                                    fileerror << lines << " b\n";
                                }
                            } else {
                                if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                                    localSymbolMap.insert(
                                            make_pair(name, Symbology(name, "array", type, arrayInfo, localAddr)));
                                    localAddr += num1;
                                } else {  //找到了 说明重定义了
                                    fileerror << lines << " b\n";
                                }
                            }
                            doOutPut();
                            getsym();
                            fileout << "<变量定义及初始化>" << endl;
                            return true;
                        }
                    }
                } else if (Symbol == LBRACK) {   //'['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
                    doOutPut();
                    getsym();
                    if (!unsignedInt()) {                    //无符号
                        fileerror << lines << " i\n";
                        getsym();
                    }
                    int num2 = num;
                    arrayInfo.push_back(num);
                    if (Symbol != RBRACK) { //不是]报错
                        retractG();
                        fileerror << lines << " m\n";
                        Symbol = RBRACK;
                    }
                    if (Symbol == RBRACK) {
                        doOutPut();
                        getsym();
                        if (Symbol == ASSIGN) {
                            doOutPut();
                            getsym();
                            midCodeTable.push_back(midCode(ArrayOp, name, type, "1"));
                            (midCodeTable.end() - 1)->insert(2);
                            (midCodeTable.end() - 1)->insert(num1);
                            (midCodeTable.end() - 1)->insert(num2);
                            if (Symbol == LBRACE) {   //'{'＜常量＞{,＜常量＞}'}'
                                doOutPut();
                                getsym();
                                if (Symbol != LBRACE) {
                                    error();
                                    return false;
                                }
                                int i = 0;
                                while (true) {
                                    i++;
                                    doOutPut();
                                    if (Symbol != LBRACE) {
                                        break;
                                    }
                                    getsym();
                                    //'{'＜常量＞{,＜常量＞}'}'
                                    if (!Const(&constType)) {
                                        error();
                                        return false;
                                    }
                                    if (constType != compareType) {
                                        fileerror << lines << " o\n";
                                    }
                                    (midCodeTable.end() - 1)->insert(toInt(nowConst));
                                    int j = 1;
                                    while (Symbol == COMMA) {
                                        j++;
                                        doOutPut();
                                        getsym();
                                        if (!Const(&constType)) {
                                            error();
                                            return false;
                                        }
                                        (midCodeTable.end() - 1)->insert(toInt(nowConst));
                                        if (constType != compareType) {
                                            fileerror << lines << " o\n";
                                        }
                                    }
                                    if (Symbol == RBRACE) {
                                        doOutPut();
                                        getsym();
                                        if (j != num2) {
                                            fileerror << lines << " n\n";
                                        }
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
                                    if (i != num1) {
                                        fileerror << lines << " n\n";
                                    }
                                    arrayInfo.insert(arrayInfo.begin(), 2);
                                    if (global) {
                                        if (globalSymbolMap.find(name) == globalSymbolMap.end()) {  //没找到
                                            globalSymbolMap.insert(
                                                    make_pair(name,
                                                              Symbology(name, "array", type, arrayInfo, globalAddr)));
                                            globalAddr += num1 * num2;
                                        } else {  //找到了 说明重定义了
                                            fileerror << lines << " b\n";
                                        }
                                    } else {
                                        if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                                            localSymbolMap.insert(
                                                    make_pair(name,
                                                              Symbology(name, "array", type, arrayInfo, localAddr)));
                                            localAddr += num1 * num2;
                                        } else {  //找到了 说明重定义了
                                            fileerror << lines << " b\n";
                                        }
                                    }
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
    return false;
}

//＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
bool valueReturnFunction() {
    hasReturn = 0;
    if (deHead(&localType)) {
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (parameterTable()) {
                if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                    Symbol = RPARENT;
                }
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    if (Symbol == LBRACE) {
                        doOutPut();
                        getsym();
                        if (compoundStatement(0)) {
                            if (Symbol == RBRACE) {
                                if (hasReturn == 0) {
                                    fileerror << lines << " h\n";
                                }
                                doOutPut();
                                getsym();
                                fileout << "<有返回值函数定义>" << endl;
                                allLocalSymbolTable.insert(make_pair(funName, localSymbolMap));
                                globalSymbolMap[funName].length = localAddr;
                                localSymbolMap.clear();
                                localAddr = 3;
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
    localType = -1;
    if (Symbol == VOIDTK) {
        getsym();
        if (Symbol == IDENFR) {
            funName.assign(inLowerToken);
            midCodeTable.push_back(midCode(FuncOp,"void", funName, ""));
            if (globalSymbolMap.find(funName) == globalSymbolMap.end()) {  //没找到
                globalSymbolMap.insert(make_pair(funName, Symbology(funName, "fun", "void")));
            } else {  //找到了 说明重定义了
                fileerror << lines << " b\n";
            }
            retractG(tmpIndex);
            getsym();
            doOutPut();
            getsym();
            doOutPut();
            getsym();
            if (Symbol == LPARENT) {
                doOutPut();
                getsym();
                if (parameterTable()) {
                    if (Symbol != RPARENT) { //不是)报错
                        retractG();
                        fileerror << lines << " l\n";
                        Symbol = RPARENT;
                    }
                    if (Symbol == RPARENT) {
                        doOutPut();
                        getsym();
                        if (Symbol == LBRACE) {
                            doOutPut();
                            getsym();
                            if (compoundStatement(0)) {
                                if (Symbol == RBRACE) {
                                    doOutPut();
                                    getsym();
                                    fileout << "<无返回值函数定义>" << endl;
                                    allLocalSymbolTable.insert(make_pair(funName, localSymbolMap));
                                    globalSymbolMap[funName].length = localAddr;
                                    localSymbolMap.clear();
                                    localAddr = 3;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        } else if (Symbol == MAINTK) {
            retractG(tmpIndex);
            getsym();
            return false;
        }
    }
    return false;
}

//＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
bool compoundStatement(int global) {
    constDeclaration(global);
    varDeclaration(global);
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
    int compareType;
    if (Symbol != CHARTK && Symbol != INTTK) {
        error();
        return false;
    } else {
        string type = inLowerToken;
        if (type.compare("int") == 0) {
            compareType = 0;
        } else {
            compareType = 1;
        }
        doOutPut();
        getsym();
        if (Symbol == IDENFR) {
            string name = inLowerToken;
            if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                localSymbolMap.insert(make_pair(name, Symbology(name, "var", type, 0, localAddr)));
                localAddr++;
                globalSymbolMap[funName].insert(compareType);
            } else {  //找到了 说明重定义了
                fileerror << lines << " b\n";
            }
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
                    type.assign(inLowerToken);
                    doOutPut();
                    getsym();
                    if (Symbol == IDENFR) {
                        string name = inLowerToken;
                        if (type.compare("int") == 0) {
                            compareType = 0;
                        } else {
                            compareType = 1;
                        }
                        if (localSymbolMap.find(name) == localSymbolMap.end()) {  //没找到
                            localSymbolMap.insert(make_pair(name, Symbology(name, "var", type, 0, localAddr ++)));
                            globalSymbolMap[funName].insert(compareType);
                        }
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
    localType = -1;
    if (Symbol == VOIDTK) {
        doOutPut();
        getsym();
        if (Symbol == MAINTK) {
            doOutPut();
            getsym();
            funName.assign("main");
            midCodeTable.push_back(midCode(MainOp, "void", "main", ""));
            if (Symbol == LPARENT) {
                doOutPut();
                getsym();
                if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                }
                doOutPut();
                getsym();
                if (Symbol == LBRACE) {
                    doOutPut();
                    getsym();
                    if (compoundStatement(0)) {
                        if (Symbol == RBRACE) {
                            doOutPut();
                            getsym();
                            fileout << "<主函数>" << endl;
                            allLocalSymbolTable.insert(make_pair("main", localSymbolMap));
                            globalSymbolMap["main"].length = localAddr;
                            localSymbolMap.clear();
                            localAddr = 3;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

//＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}   //[+|-]只作用于第一个<项>
bool expression(int *t, string &value) {
    bool first = false;
    int re;
    bool isPLUS;
    if (Symbol == PLUS || Symbol == MINU) {
        isPLUS = (Symbol == PLUS);
        doOutPut();
        re = getsym();
        if (re < 0) {
            return false;
        }
        first = true;
    }
    //进入项的分析
    string op1, op2, res;
    if (!item(t, op1)) {
        return false;
    }
    if (first) {  //第一项前边有正负号
        if (!isPLUS) {  //减号
            res = genTmp();
            localSymbolMap.insert(make_pair(res, Symbology(res, "var", "int", 0, localAddr)));  //kind=1=var,type=1=int
            localAddr++;
            midCodeTable.push_back(midCode(MinuOp, res, toString(0), op1));
            op1 = res;
        }
    }
    //项分析成功 并预读了一个单词
    bool flag = false;
    //开始分析{＜加法运算符＞＜项＞}
    while (true) {
        if (isEOF()) {
            break;
        }
        if (Symbol != PLUS && Symbol != MINU) {  //不是+不是-
            break;
        }
        //项中包括了加法减法 就一定是int了
        isPLUS = (Symbol == PLUS);
        flag = true;
        doOutPut();
        re = getsym();
        if (re < 0) {
            return false;
        }
        //进入项的分析
        if (!item(t, op2)) {
            return false;
        }
        res = genTmp();
        localSymbolMap.insert(make_pair(res, Symbology(res, "var", "int", 0, localAddr)));  //kind=1=var,type=1=int
        localAddr++;
        midCodeTable.push_back(midCode(isPLUS ? PlusOp : MinuOp, res, op1, op2));
        op1 = res;
    }
    if (first) {  //带有最前边的+-号 一定是int
        *t = 0;
    } else {
        if (flag) {  //项中包括了加法减法 就一定是int
            *t = 0;
        }
    }
    value = op1;
    fileout << "<表达式>" << endl;
    return true;
}

//＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
bool item(int *t, string &value) {
    string op1, op2, res;
    if (!factor(t, op1)) {  //直接分析因子
        return false;
    }
    //因子分析成功 并预读了一个单词
    //开始分析 {＜乘法运算符＞＜因子＞}
    bool flag = false;
    bool isMULT;
    while (true) {
        if (isEOF()) {
            break;
        }
        if (Symbol != MULT && Symbol != DIV) {  //不是* 也不是/
            break;
        }
        isMULT = (Symbol == MULT);
        doOutPut();
        //项中包括了乘法除法 就一定是int了
        flag = true;
        int re = getsym();
        if (re < 0) {
            return false;
        }
        //进入因子的分析
        if (!factor(t, op2)) {
            return false;
        }
        res = genTmp();
        //string name, string kind, string type, int value, int addr
        localSymbolMap.insert(make_pair(res, Symbology(res, "var", "int", 0, localAddr)));  //kind=1=var,type=1=int
        localAddr++;
        midCodeTable.push_back(midCode(isMULT ? MultOp : DivOp, res, op1, op2));
        op1 = res;
    }
    if (flag) {  //项中包括了乘法除法 就一定是int了
        *t = 0;
    }
    value = op1;
    fileout << "<项>" << endl;
    return true;
}

//＜因子＞    ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'|＜标识符＞'['＜表达式＞']''['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
bool factor(int *t, string &value) {
    int tmpIndex = indexs - token.length();
    int type;
    string nameTmp, name;
    if (Symbol == IDENFR) {  //当前是 ＜标识符＞｜＜标识符＞'['＜表达式＞']'|＜标识符＞'['＜表达式＞']''['＜表达式＞']' 也可能是 ＜有返回值函数调用语句＞
        nameTmp.assign(token);
        name.assign(inLowerToken);
        getsym();
        if (Symbol == LBRACK) {  //是[
            if (localSymbolMap.find(name) != localSymbolMap.end() &&
                localSymbolMap[name].kind.compare("array") == 0) { //数组类型
                if (localSymbolMap[name].type.compare("int") == 0) {
                    type = 0;
                } else {
                    type = 1;
                }
            } else {
                if (globalSymbolMap.find(name) != globalSymbolMap.end() &&
                    globalSymbolMap[name].kind.compare("array") == 0) { //数组类型kind=4
                    if (globalSymbolMap[name].type.compare("int") == 0) {
                        type = 0;
                    } else {
                        type = 1;
                    }
                } else {
                    fileerror << lines << " c\n";
                }
            }
            Symbol = IDENFR;
            token.assign(nameTmp);
            doOutPut();
            Symbol = LBRACK;
            token.assign("[");
            doOutPut();
            getsym();
            string op1 = genTmp();
            localSymbolMap.insert(
                    make_pair(op1, Symbology(op1, "var", "", type, localAddr ++)));
            if (!expression(t, op1)) {
                error();
                return false;
            }
            if (*t != 0) {
                fileerror << lines << " i\n";
            }
            //表达式分析成功 并预读了一个单词
            if (Symbol != RBRACK) { //不是]报错
                retractG();
                fileerror << lines << " m\n";
                Symbol = RBRACK;
            }
            if (Symbol == RBRACK) {  //是]
                *t = type;
                doOutPut();
                getsym();   //为下一个预读 不管是啥
                string op2 = genTmp();
                localSymbolMap.insert(
                        make_pair(op2, Symbology(op2, "var", "", type, localAddr)));  //kind=1=var,type=数组的type
                localAddr++;
                if (Symbol != LBRACK) {
                    midCodeTable.push_back(midCode(GetArray, op2, name, "1"));
                    (midCodeTable.end() - 1)->info(op1);
                    value = op2;
                    fileout << "<因子>" << endl;
                    return true;
                } else {            //[
                    doOutPut();
                    getsym();
                    if (!expression(t, op2)) {
                        error();
                        return false;
                    }
                    if (*t != 0) {
                        fileerror << lines << " i\n";
                    }
                    if (Symbol != RBRACK) { //不是]报错
                        retractG();
                        fileerror << lines << " m\n";
                        Symbol = RBRACK;
                    }
                    if (Symbol == RBRACK) {
                        *t = type;
                        doOutPut();
                        getsym();
                        value = genTmp();
                        localSymbolMap.insert(make_pair(value, Symbology(value, "var", "", type,
                                                                         localAddr)));  //kind=1=var,type=数组的type
                        localAddr++;
                        midCodeTable.push_back(midCode(GetArray, value, name, "2"));
                        (midCodeTable.end() - 1)->info(op1);
                        (midCodeTable.end() - 1)->info(op2);
                        fileout << "<因子>" << endl;
                        return true;
                    }
                }
            } else {
                return false;
            }
        } else if (Symbol == LPARENT) {  //是( 说明这个是  ＜有返回值函数调用语句＞  需要恢复
            if (globalSymbolMap.find(name) != globalSymbolMap.end()) {
                if (globalSymbolMap[name].kind.compare("fun") != 0) {
                    fileerror << lines << " c\n";  //未定义的名字
                } else {
                    if (globalSymbolMap[name].type.compare("void") == 0) {
                        type = -1;
                    } else if (globalSymbolMap[name].type.compare("int") == 0) {
                        type = 0;
                    } else {
                        type = 1;
                    }
                }
            } else {
                fileerror << lines << " c\n";  //未定义的名字
            }
            retractG(tmpIndex);
            getsym();   //把标识符重新读出来
            //开始调用 有返回值的函数调用语句
            if (type == -1) {
                if (callNoValueReturnFunction()) {
                    *t = -1;
                    //调用有返回值的函数调用语句成功 并预读了一个单词
                    fileout << "<因子>" << endl;
                    return true;
                }
            } else {
                if (callValueReturnFunction()) {
                    *t = type;
                    string op1 = genTmp();
                    localSymbolMap.insert(make_pair(op1, Symbology(op1, "var", "", type, localAddr ++)));
                    midCodeTable.push_back(midCode(ReturnValue, op1, name, ""));
                    value = op1;
                    //调用有返回值的函数调用语句成功 并预读了一个单词
                    fileout << "<因子>" << endl;
                    return true;
                }
            }
        } else {  //标识符后边不是[ 也不是(  对应文法＜标识符＞  直接返回 这个单词就是为下一个预读的单词了
            if (localSymbolMap.find(name) != localSymbolMap.end() &&
                localSymbolMap[name].kind.compare("fun") != 0) { //数组类型
                if (localSymbolMap[name].type.compare("int") == 0) {
                    type = 0;
                } else {
                    type = 1;
                }
            } else {
                if (globalSymbolMap.find(name) != globalSymbolMap.end() &&
                    globalSymbolMap[name].kind.compare("fun") != 0) { //数组类型kind=4
                    if (globalSymbolMap[name].type.compare("int") == 0) {
                        type = 0;
                    } else {
                        type = 1;
                    }
                } else {
                    fileerror << lines << " c\n";
                }
            }
            retractG(tmpIndex);
            *t = type;
            value = name;
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
        if (!expression(t, value)) {
            return false;
        }
        if (Symbol != RPARENT) { //不是)报错
            retractG();
            fileerror << lines << " l\n";
            Symbol = RPARENT;
        }
        if (Symbol == RPARENT) {  //)
            doOutPut();
            getsym();   //为下一个预读 不管是啥
            fileout << "<因子>" << endl;
            *t != -1 ? *t = 0 : *t = -1;
            return true;
        } else {
            return false;
        }
    } else if (Symbol == CHARCON) {  //当前是字符 对应文法 ＜字符＞
        value.assign(toString((int) con_ch));
        *t = 1;
        doOutPut();
        getsym();   //预读
        fileout << "<因子>" << endl;
        return true;
    } else if (Int()) {  //整数
        value = toString(num);
        *t = 0;
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
            if (Symbol != SEMICN) { //不是；报错
                retractG();
                fileerror << lines << " k\n";
                Symbol = SEMICN;
            }
            if (Symbol == SEMICN) {  //;分号
                doOutPut();
                fileout << "<语句>" << endl;
                getsym();
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
            if (Symbol != SEMICN) { //不是；报错
                retractG();
                fileerror << lines << " k\n";
                Symbol = SEMICN;
            }
            if (Symbol == SEMICN) {  //;分号
                doOutPut();
                fileout << "<语句>" << endl;
                getsym();
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
            if (Symbol != SEMICN) { //不是；报错
                retractG();
                fileerror << lines << " k\n";
                Symbol = SEMICN;
            }
            if (Symbol == SEMICN) {  //;分号
                doOutPut();
                fileout << "<语句>" << endl;
                getsym();
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
    } else if (Symbol == SWITCHTK) {  //＜情况语句＞
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
        string name;
        name.assign(inLowerToken);
        int tmpIndex = indexs - token.length();
        getsym();
        if (Symbol == LBRACK || Symbol == ASSIGN) {  //[和= 说明是赋值语句
            retractG(tmpIndex);
            getsym();
            if (!assignStatement()) {
                return false;
            }
            //分析赋值语句成功 并预读了一个单词
            if (Symbol != SEMICN) { //不是；报错
                retractG();
                fileerror << lines << " k\n";
                Symbol = SEMICN;
            }
            if (Symbol == SEMICN) {  //;分号
                doOutPut();
                fileout << "<语句>" << endl;
                getsym();
                return true;
            }
        } else if (Symbol == LPARENT) {  //( 说明是函数调用语句
            retractG(tmpIndex);             //回退到读标志符前
            getsym();
            if (!globalSymbolMap[name].type.compare("void")) {
                if (!callNoValueReturnFunction()) {
                    return false;
                }
                if (Symbol != SEMICN) { //不是；报错
                    retractG();
                    fileerror << lines << " k\n";
                    Symbol = SEMICN;
                }
                if (Symbol == SEMICN) {  //;分号
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
                if (Symbol != SEMICN) { //不是；报错
                    retractG();
                    fileerror << lines << " k\n";
                    Symbol = SEMICN;
                }
                if (Symbol == SEMICN) {  //;分号
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
    error();
    return false;
}

//＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞|＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
bool assignStatement() {
    int type, idType;
    string value, op1, op2;
    if (Symbol == IDENFR) {
        string name;
        name.assign(inLowerToken);
        if (localSymbolMap.find(name) != localSymbolMap.end() && localSymbolMap[name].kind.compare("fun") != 0) {
            if (!localSymbolMap[name].kind.compare("const")) {  //const
                fileerror << lines << " j\n";  //改变常量的值了
            }
            idType = localSymbolMap[name].type.compare("int") ? 0 : 1;
        } else if (globalSymbolMap.find(name) != globalSymbolMap.end() &&
                   globalSymbolMap[name].kind.compare("fun") != 0) {
            if (!globalSymbolMap[name].kind.compare("const")) {  //const
                fileerror << lines << " j\n";  //改变常量的值了
            }
            idType = globalSymbolMap[name].type.compare("int") ? 0 : 1;
        } else {
            fileerror << lines << " c\n";  //未定义的名字
        }
        doOutPut();
        getsym();
        if (Symbol == ASSIGN) {       //第一种情况
            doOutPut();
            getsym();
            if (expression(&type, value)) {
                midCodeTable.push_back(midCode(AssignOp, name, value, ""));
                fileout << "<赋值语句>" << endl;
                return true;
            }
        } else if (Symbol == LBRACK) {
            doOutPut();
            getsym();
            op1 = genTmp();
            localSymbolMap.insert(make_pair(op1, Symbology(op1, "var", "int", 0, localAddr)));  //kind=1=var,type=1=int
            localAddr++;
            if (expression(&type, op1)) {
                if (type != 0) {
                    fileerror << lines << " i\n";
                }
                if (Symbol != RBRACK) { //不是]报错
                    retractG();
                    fileerror << lines << " m\n";
                    Symbol = RBRACK;
                }
                if (Symbol == RBRACK) {
                    doOutPut();
                    getsym();
                    if (Symbol == ASSIGN) {   //第二种情况
                        doOutPut();
                        getsym();
                        if (expression(&type, value)) {
                            midCodeTable.push_back(midCode(PutArray, value, name, "1"));
                            (midCodeTable.end() - 1)->info(op1);
                            fileout << "<赋值语句>" << endl;
                            return true;
                        }
                    } else if (Symbol == LBRACK) {
                        doOutPut();
                        getsym();
                        op2 = genTmp();
                        localSymbolMap.insert(
                                make_pair(op2, Symbology(op2, "var", "int", 0, localAddr)));  //kind=1=var,type=1=int
                        localAddr++;
                        if (expression(&type, op2)) {
                            if (type != 0) {
                                fileerror << lines << " i\n";
                            }
                            if (Symbol != RBRACK) { //不是]报错
                                retractG();
                                fileerror << lines << " m\n";
                                Symbol = RBRACK;
                            }
                            if (Symbol == RBRACK) {
                                doOutPut();
                                getsym();
                                if (Symbol == ASSIGN) {   //第三种情况
                                    doOutPut();
                                    getsym();
                                    if (expression(&type, value)) {
                                        midCodeTable.push_back(midCode(PutArray, value, name, "2"));
                                        (midCodeTable.end() - 1)->info(op1);
                                        (midCodeTable.end() - 1)->info(op2);
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
    string label, labelOut;
    labelOut.assign("label" + toString(labelMark++));
    if (Symbol == IFTK) {  //if
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {  //(
            doOutPut();
            getsym();
            if (!condition(label)) {  //分析条件
                return false;
            }
            if (Symbol != RPARENT) { //不是)报错
                retractG();
                fileerror << lines << " l\n";
                Symbol = RPARENT;
            }
            if (Symbol == RPARENT) {  //)
                doOutPut();
                getsym();
                if (!Statement()) {  //分析语句
                    return false;
                }
                //goto labelOut
                midCodeTable.push_back(midCode(GoTo, labelOut, "", ""));
                midCodeTable.push_back(midCode(Label, label, "", ""));
                //开始分析［else＜语句＞］
                if (Symbol == ELSETK) {  //else
                    //label
                    doOutPut();
                    getsym();
                    if (!Statement()) {  //分析语句
                        return false;
                    }
                    //labelOut
                }
                midCodeTable.push_back(midCode(Label, labelOut, "", ""));
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
bool condition(string &res) {
    int type1, type2;
    string value1, value2;
    if (expression(&type1, value1)) {
        if (type1 != 0) {
            fileerror << lines << " f\n";
        }
        if (Relation()) {
            mid_operation op;
            switch (Symbol) {
                case LSS:
                    op = LssOp;
                    break;
                case LEQ:
                    op = LeqOp;
                    break;
                case GRE:
                    op = GreOp;
                    break;
                case GEQ:
                    op = GeqOp;
                    break;
                case EQL:
                    op = BeqOp;
                    break;
                case NEQ:
                    op = NeqOp;
                    break;
            }
            getsym();
            if (expression(&type2, value2)) {
                if (type2 != 0) {
                    fileerror << lines << " f\n";
                }
                res.assign("else" + toString(labelMark++));
                midCodeTable.push_back(midCode(op, res, value1, value2));
                fileout << "<条件>" << endl;
                return true;
            }
        }
    }
    return false;
}

//＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞| for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
bool loopStatement() {
    int type;
    string value, labelOut, label;
    labelOut.assign("label" + toString(labelMark++));
    label.assign("label" + toString(labelMark++));
    if (Symbol == WHILETK) {    //while
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            midCodeTable.push_back(midCode(Label, label, "", ""));
            if (condition(labelOut)) {
                if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                    Symbol = RPARENT;
                }
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    if (Statement()) {
                        //labelOut
                        midCodeTable.push_back(midCode(GoTo, label, "", ""));
                        midCodeTable.push_back(midCode(Label, labelOut, "", ""));
                        fileout << "<循环语句>" << endl;
                        return true;
                    }
                }
            }
        }
    } else if (Symbol == FORTK) {    //for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
        string label2, label3, change;
        change = genTmp();
        localSymbolMap.insert(
                make_pair(change, Symbology(change, "var", "", type, localAddr)));  //kind=1=var,type=数组的type
        localAddr++;
        label2.assign("label" + toString(labelMark++));
        label3.assign("label" + toString(labelMark++));
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (Symbol == IDENFR) {
                string name = inLowerToken;
                if (localSymbolMap.find(name) != localSymbolMap.end() &&
                    localSymbolMap[name].kind.compare("fun") != 0) {
                    if (localSymbolMap[name].kind.compare("const") == 0) {  //const
                        fileerror << lines << " j\n";  //改变常量的值了
                    }
                } else {
                    if (globalSymbolMap.find(name) != globalSymbolMap.end() &&
                        globalSymbolMap[name].kind.compare("fun") != 0) {
                        if (globalSymbolMap[name].kind.compare("const") == 0) {  //const
                            fileerror << lines << " j\n";  //改变常量的值了
                        }
                    } else {
                        fileerror << lines << " c\n";  //未定义的名字
                    }
                }
                doOutPut();
                getsym();
                if (Symbol == ASSIGN) {
                    doOutPut();
                    getsym();
                    if (expression(&type, value)) {
                        midCodeTable.push_back(midCode(AssignOp, name, value, ""));
                        if (Symbol != SEMICN) { //不是;报错
                            retractG();
                            fileerror << lines << " k\n";
                            Symbol = SEMICN;
                        }
                        if (Symbol == SEMICN) {
                            doOutPut();
                            getsym();
                            midCodeTable.push_back(midCode(Label, label, "", ""));
                            if (condition(labelOut)) {
                                if (Symbol != SEMICN) { //不是;报错
                                    retractG();
                                    fileerror << lines << " k\n";
                                    Symbol = SEMICN;
                                }
                                if (Symbol == SEMICN) {
                                    midCodeTable.push_back(midCode(GoTo, label3, "", ""));
                                    midCodeTable.push_back(midCode(Label, label2, "", ""));
                                    doOutPut();
                                    getsym();
                                    if (Symbol == IDENFR) {
                                        string name1 = inLowerToken;
                                        if (localSymbolMap.find(name1) != localSymbolMap.end() &&
                                            localSymbolMap[name1].kind.compare("fun") != 0) {
                                            if (localSymbolMap[name1].kind.compare("const") == 0) {  //const
                                                fileerror << lines << " j\n";  //改变常量的值了
                                            }
                                        } else {
                                            if (globalSymbolMap.find(name1) != globalSymbolMap.end() &&
                                                globalSymbolMap[name1].kind.compare("fun") != 0) {
                                                if (globalSymbolMap[name1].kind.compare("const") == 0) {  //const
                                                    fileerror << lines << " j\n";  //改变常量的值了
                                                }
                                            } else {
                                                fileerror << lines << " c\n";  //未定义的名字
                                            }
                                        }
                                        doOutPut();
                                        getsym();
                                        if (Symbol == ASSIGN) {
                                            doOutPut();
                                            getsym();
                                            if (Symbol == IDENFR) {
                                                string name = inLowerToken;
                                                if (localSymbolMap.find(name) != localSymbolMap.end() &&
                                                    localSymbolMap[name].kind.compare("fun") != 0) {
                                                    if (localSymbolMap[name].kind.compare("const") == 0) {  //const
                                                        fileerror << lines << " j\n";  //改变常量的值了
                                                    }
                                                } else {
                                                    if (globalSymbolMap.find(name) != globalSymbolMap.end() &&
                                                        globalSymbolMap[name].kind.compare("fun") != 0) {
                                                        if (globalSymbolMap[name].kind.compare("const") == 0) {  //const
                                                            fileerror << lines << " j\n";  //改变常量的值了
                                                        }
                                                    } else {
                                                        fileerror << lines << " c\n";  //未定义的名字
                                                    }
                                                }
                                                doOutPut();
                                                getsym();
                                                if (Symbol == PLUS || Symbol == MINU) {
                                                    symbol tmpSymbol = Symbol;
                                                    doOutPut();
                                                    getsym();
                                                    if (step()) {
                                                        if (tmpSymbol == PLUS) {
                                                            midCodeTable.push_back(
                                                                    midCode(PlusOp, change, name, toString(num)));
                                                        } else {
                                                            midCodeTable.push_back(
                                                                    midCode(MinuOp, change, name, toString(num)));
                                                        }
                                                        midCodeTable.push_back(midCode(AssignOp, name, change, ""));
                                                        midCodeTable.push_back(midCode(GoTo, label, "", ""));
                                                        if (Symbol != RPARENT) { //不是)报错
                                                            retractG();
                                                            fileerror << lines << " l\n";
                                                            Symbol = RPARENT;
                                                        }
                                                        if (Symbol == RPARENT) {
                                                            doOutPut();
                                                            getsym();
                                                            midCodeTable.push_back(midCode(Label, label3, "", ""));
                                                            if (Statement()) {
                                                                midCodeTable.push_back(midCode(GoTo, label2, "", ""));
                                                                midCodeTable.push_back(
                                                                        midCode(Label, labelOut, "", ""));
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
    int type;
    string value;
    string compare, otherwise, labelOut;
    compare.assign("label" + toString(labelMark++));
    labelOut.assign("label" + toString(labelMark++));
    otherwise.assign(labelOut);
    if (Symbol == SWITCHTK) {
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (expression(&type, value)) {
                midCodeTable.push_back(midCode(GoTo, compare, "", ""));
                if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                    Symbol = RPARENT;
                }
                if (Symbol == RPARENT) {
                    doOutPut();
                    getsym();
                    if (Symbol == LBRACE) {
                        doOutPut();
                        getsym();
                        if (caseTable(type, value, labelOut)) {
                            if (defaultStatement(otherwise)) {
                                if (Symbol == RBRACE) {
                                    midCodeTable.push_back(midCode(Label, compare, "", ""));
                                    for (CASEInfo Case : caseInfo) {
                                        midCodeTable.push_back(
                                                midCode(NeqOp, Case.label, value, toString(Case.value)));
                                    }
                                    midCodeTable.push_back(midCode(GoTo, otherwise, "", ""));
                                    midCodeTable.push_back(midCode(Label, labelOut, "", ""));
                                    doOutPut();
                                    getsym();
                                    fileout << "<情况语句>" << endl;
                                    caseInfo.clear();
                                    return true;
                                } else {
                                    fileerror << lines << " p\n";
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
bool caseTable(int type, string &value, string &label) {
    while (caseStatement(type, value)) {
        midCodeTable.push_back(midCode(GoTo, label, "", ""));
    }
    fileout << "<情况表>" << endl;
    return true;
}

//＜情况子语句＞  ::=  case＜常量＞：＜语句＞
bool caseStatement(int type, string &value) {
    int constType;
    int constValue;
    string label;
    label.assign("label" + toString(labelMark++));
    if (Symbol == CASETK) {
        doOutPut();
        getsym();
        if (Const(&constType)) {
            if (constType != type) {
                fileerror << lines << " o\n";
            }
            constValue = toInt(nowConst);
            if (Symbol == COLON) {
                doOutPut();
                getsym();
                midCodeTable.push_back(midCode(Label, label, "", ""));
                if (Statement()) {
                    caseInfo.push_back(CASEInfo(label, constValue));
                    fileout << "<情况子语句>" << endl;
                    return true;
                }
            }
        }
    }
    return false;
}

//＜缺省＞   ::=  default :＜语句＞
bool defaultStatement(string &labelOut) {
    string label;
    label.assign("label" + toString(labelMark++));
    if (Symbol == DEFAULTTK) {
        doOutPut();
        getsym();
        if (Symbol == COLON) {
            doOutPut();
            getsym();
            midCodeTable.push_back(midCode(Label, label, "", ""));
            if (Statement()) {
                midCodeTable.push_back(midCode(GoTo, labelOut, "", ""));
                labelOut.assign(label);
                fileout << "<缺省>" << endl;
                return true;
            }
        }
    }
    return false;
}

//＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
bool callValueReturnFunction() {
    string lastFun = funName;
    if (Symbol == IDENFR) {
        funName.assign(inLowerToken);
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (valueParameterTable(funName)) {
                if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                    Symbol = RPARENT;
                }
                if (Symbol == RPARENT) {
                    midCodeTable.push_back(midCode(Call, funName, "", ""));
                    doOutPut();
                    getsym();
                    funName.assign(lastFun);
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
    string lastFun = funName;
    if (Symbol == IDENFR) {
        funName.assign(inLowerToken);
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            doOutPut();
            getsym();
            if (valueParameterTable(funName)) {
                if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                    Symbol = RPARENT;
                }
                if (Symbol == RPARENT) {
                    midCodeTable.push_back(midCode(Call, funName, "", ""));
                    doOutPut();
                    getsym();
                    funName.assign(lastFun);
                    fileout << "<无返回值函数调用语句>" << endl;
                    return true;
                }
            }
        }
    }
    return false;
}

//＜值参数表＞  ::= ＜表达式＞{,＜表达式＞}｜＜空＞
bool valueParameterTable(const string &Name) {
    int type;
    string value;
    int i = 0;
    bool flag = false;
    string tmpName;
    tmpName.assign(funName);
    if (Symbol == RPARENT) {                //如果直接为右括号，则为空
        if (globalSymbolMap.find(tmpName) != globalSymbolMap.end() &&
            !globalSymbolMap[tmpName].otherInfo.empty()) {  //不是空参数表
            fileerror << lines << " d\n";
        }
        fileout << "<值参数表>" << endl;
        return true;
    } else if (expression(&type, value)) {
        i++;
        if (globalSymbolMap.find(tmpName) != globalSymbolMap.end() && i <= globalSymbolMap[tmpName].otherInfo.size()) {
            if (globalSymbolMap[tmpName].otherInfo[i - 1] != type) {  //不是相同类型
                flag = true;
            }
        }
        midCodeTable.push_back(midCode(PushOp, value, "", ""));
        if (Symbol != COMMA) {              //不是逗号退出循环
            if (globalSymbolMap.find(tmpName) != globalSymbolMap.end() &&
                i != globalSymbolMap[tmpName].otherInfo.size()) {
                fileerror << lines << " d\n";
            } else if (flag) {
                fileerror << lines << " e\n";
            }
            fileout << "<值参数表>" << endl;
            return true;
        } else {
            doOutPut();
            getsym();
            if (!expression(&type, value)) {
                error();
                return false;
            }
            do {          //表达式和，循环
                i++;
                if (globalSymbolMap.find(tmpName) != globalSymbolMap.end() &&
                    i <= globalSymbolMap[tmpName].otherInfo.size()) {
                    if (globalSymbolMap.find(tmpName) != globalSymbolMap.end() &&
                        globalSymbolMap[tmpName].otherInfo[i - 1] != type) {  //不是相同类型
                        flag = true;
                    }
                }
                midCodeTable.push_back(midCode(PushOp, value, "", ""));
                if (Symbol != COMMA) {
                    if (globalSymbolMap.find(tmpName) != globalSymbolMap.end() &&
                        i != globalSymbolMap[tmpName].otherInfo.size()) {
                        fileerror << lines << " d\n";
                    } else if (flag) {
                        fileerror << lines << " e\n";
                    }
                    fileout << "<值参数表>" << endl;
                    return true;
                }
                doOutPut();
                getsym();
            } while (expression(&type, value));
        }
    }
    if (Symbol != RPARENT) { //不是)报错
        retractG();
        Symbol = RPARENT;
    }
    if (Symbol == RPARENT) {
        doOutPut();
        getsym();
        if (globalSymbolMap.find(tmpName) != globalSymbolMap.end() &&
            globalSymbolMap[tmpName].otherInfo.size() != 0) {  //不是空参数表
            fileerror << lines << " d\n";
        }
        fileout << "<有返回值函数调用语句>" << endl;
        return true;
    }
    error();
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

//＜读语句＞    ::=  scanf '('＜标识符＞')’
bool readStatement() {
    string name;
    if (Symbol == SCANFTK) {        //scanf
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {    //(
            doOutPut();
            getsym();
            if (Symbol == IDENFR) { //标识符
                name.assign(inLowerToken);
                if (localSymbolMap.find(name) != localSymbolMap.end() &&
                    localSymbolMap[name].kind.compare("fun") != 0) {
                    if (!localSymbolMap[name].kind.compare("const")) {  //const
                        fileerror << lines << " j\n";  //改变常量的值了
                    }
                } else if (globalSymbolMap.find(name) != globalSymbolMap.end() &&
                           globalSymbolMap[name].kind.compare("fun") != 0) {
                    if (!globalSymbolMap[name].kind.compare("const")) {  //const
                        fileerror << lines << " j\n";  //改变常量的值了
                    }
                } else {
                    fileerror << lines << " c\n";  //未定义的名字
                }
                midCodeTable.push_back(midCode(ScanOp, name, "", ""));
                doOutPut();
                getsym();
                if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                    Symbol = RPARENT;
                    if (Symbol == RPARENT) {    //）
                        doOutPut();
                        getsym();
                        fileout << "<读语句>" << endl;
                        return true;
                    }
                } else if (Symbol == RPARENT) {         //）
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
    int type;
    string value;
    if (Symbol == PRINTFTK) {                           //printf
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {                        //(
            doOutPut();
            getsym();
            string tmpString;
            if (String(tmpString)) {                         //字符串
                int p = -2;
                while ((p = tmpString.find("\\", p + 2)) != string::npos) {
                    tmpString.replace(p, 1, "\\\\");
                }
                stringList.push_back(tmpString);
                midCodeTable.push_back(midCode(PrintOp, tmpString, "0", ""));
                if (Symbol == COMMA) {                          //,
                    doOutPut();
                    getsym();
                    if (expression(&type, value)) {                          //表达式
                        if (Symbol != RPARENT) { //不是)报错
                            retractG();
                            fileerror << lines << " l\n";
                            Symbol = RPARENT;
                        }
                        if (Symbol == RPARENT) {                //）
                            doOutPut();
                            getsym();
                            midCodeTable.push_back(midCode(PrintOp, value, type == 0 ? "2" : "3", ""));
                            midCodeTable.push_back(midCode(PrintOp, "EndLine", "1", ""));
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
                } else if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                    Symbol = RPARENT;
                    if (Symbol == RPARENT) {         //）
                        doOutPut();
                        getsym();
                        midCodeTable.push_back(midCode(PrintOp, "EndLine", "1", ""));
                        fileout << "<写语句>" << endl;
                        return true;
                    }
                } else if (Symbol == RPARENT) {         //）
                    midCodeTable.push_back(midCode(PrintOp, "EndLine", "1", ""));
                    doOutPut();
                    getsym();
                    fileout << "<写语句>" << endl;
                    return true;
                }
            } else if (expression(&type, value)) {                       //表达式
                if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                    Symbol = RPARENT;
                }
                if (Symbol == RPARENT) {                    //）
                    midCodeTable.push_back(midCode(PrintOp, value, type == 0 ? "2" : "3", ""));
                    midCodeTable.push_back(midCode(PrintOp, "EndLine", "1", ""));
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
    int type;
    string value;
    if (Symbol == RETURNTK) {
        doOutPut();
        getsym();
        if (Symbol == LPARENT) {
            if (localType == -1) {
                fileerror << lines << " g\n";
            }
            doOutPut();
            getsym();
            if (Symbol == RPARENT) {
                if (localType != -1) {
                    hasReturn = 1;
                    fileerror << lines << " h\n";
                }
                doOutPut();
                getsym();
                fileout << "<返回语句>" << endl;
                return true;
            } else if (expression(&type, value)) {
                if (localType != type && localType != -1) {
                    hasReturn = 1;
                    fileerror << lines << " h\n";
                } else if (localType == type && localType != -1) {
                    hasReturn = 1;
                }
                if (Symbol != RPARENT) { //不是)报错
                    retractG();
                    fileerror << lines << " l\n";
                    Symbol = RPARENT;
                }
                if (Symbol == RPARENT) {
                    midCodeTable.push_back(midCode(ReturnOp, value, "", ""));
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
            if (localType != -1) {
                hasReturn = 1;
                fileerror << lines << " h\n";
            }
            if (funName.compare("main") == 0) {
                midCodeTable.push_back(midCode(Exit, "", "", ""));
            } else {
                midCodeTable.push_back(midCode(ReturnOp, "", "", ""));
            }
            fileout << "<返回语句>" << endl;
            return true;
        } else if (Symbol != SEMICN) { //不是;报错
            retractG();
            if (hasReturn == 0) {
                fileerror << lines << " h\n";
            }
            fileerror << lines << " k\n";
            Symbol = SEMICN;
            if (Symbol == SEMICN) {
                fileout << "<返回语句>" << endl;
                return true;
            }
        }
    }
    error();
    return false;
}

//关系运算符
bool Relation() {
    if (Symbol == LSS || Symbol == LEQ || Symbol == GRE || Symbol == GEQ || Symbol == EQL || Symbol == NEQ) {
        doOutPut();
        return true;
    } else {
        return false;
    }
}

void retractG() {
    indexs = oldIndexs; //回退
    for (int i = indexs; i < indexs + token.length(); i++) {
        if (testfile[i] == '\n') {
            lines--;
        }
    }
}

void retractG(int oldindex) {
    int tmp = indexs;
    indexs = oldindex; //回退
    for (int i = indexs; i < tmp; i++) {
        if (testfile[i] == '\n') {
            lines--;
        }
    }
}

string genTmp() {
    tmpVarId++;
    return "Temp" + toString(tmpVarId);
}