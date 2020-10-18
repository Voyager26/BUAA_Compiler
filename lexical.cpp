#include <iostream>
#include <string>
#include "lexical.h"
#include "globalVar.h"
using namespace std;

int getsym() {
    clearToken();
    getch();
    if (isEOF()) {
        clearToken();
        Symbol = null;
        return -1;
    }
    while (isBlank(ch)) {    //持续读取空白字符
        getch();
    }
    if (isLetter(ch)) {
        while (isLetter(ch) || isDigit(ch)) {
            catToken();
            catLowerToken();
            getch();
        }
        retract();          //读取连续字符串结束
        int resultValue = isReserved();
        Symbol = resultValue == -1 ? IDENFR : static_cast<symbol>(resultValue);
        return static_cast<int>(Symbol);
    } else if (isDigit(ch)) {
        while (isDigit(ch)) {
            catToken();
            getch();
        }
        retract();
        num = transNum();
        Symbol = INTCON;
        return static_cast<int>(Symbol);
    } else if (isSinglequo(ch)) {
        getch();
        if (isChar(ch)) {
            char tmp = ch;
            getch();
            if (isSinglequo(ch)) {
                token = tmp;
                Symbol = CHARCON;
                return static_cast<int>(Symbol);
            }
        } else {
            cout << "error in initial char!" << endl;
        }
    } else if (isDoublequo(ch)) {
        getch();
        string tmp;
        while (isChar(ch)) {
            tmp.append(1, ch);
            getch();
        }
        if (isDoublequo(ch)) {
            token = tmp;
            Symbol = STRCON;
            return static_cast<int>(Symbol);
        } else {
            cout << "error in initial string!" << endl;
        }
    } else if (isLss(ch)) {
        string tmp;
        tmp.append(1, ch);
        getch();
        if (isEqu(ch)) {
            tmp.append(1, ch);
            token = tmp;
            Symbol = LEQ;
            return static_cast<int>(Symbol);
        } else {
            token = tmp;
            Symbol = LSS;
            retract();
            return static_cast<int>(Symbol);
        }
    } else if (isGre(ch)) {
        string tmp;
        tmp.append(1, ch);
        getch();
        if (isEqu(ch)) {
            tmp.append(1, ch);
            token = tmp;
            Symbol = GEQ;
            return static_cast<int>(Symbol);
        } else {
            token = tmp;
            Symbol = GRE;
            retract();
            return static_cast<int>(Symbol);
        }
    } else if (isExcla(ch)) {
        string tmp;
        tmp.append(1, ch);
        getch();
        if (isEqu(ch)) {
            tmp.append(1, ch);
            token = tmp;
            Symbol = NEQ;
            return static_cast<int>(Symbol);
        } else {
            cout << "error in NEQ!" << endl;
        }
    } else if (isPlus(ch)) {
        Symbol = PLUS;
        token = "+";
        return static_cast<int>(Symbol);
    } else if (isMinu(ch)) {
        Symbol = MINU;
        token = "-";
        return static_cast<int>(Symbol);
    } else if (isMult(ch)) {
        Symbol = MULT;
        token = "*";
        return static_cast<int>(Symbol);
    } else if (isDiv(ch)) {
        Symbol = DIV;
        token = "/";
        return static_cast<int>(Symbol);
    } else if (isColon(ch)) {
        Symbol = COLON;
        token = ":";
        return static_cast<int>(Symbol);
    } else if (isSemicn(ch)) {
        Symbol = SEMICN;
        token = ";";
        return static_cast<int>(Symbol);
    } else if (isComma(ch)) {
        Symbol = COMMA;
        token = ",";
        return static_cast<int>(Symbol);
    } else if (isEqu(ch)) {
        string tmp;
        tmp.append(1, ch);
        getch();
        if (isEqu(ch)) {
            tmp.append(1, ch);
            token = tmp;
            Symbol = EQL;
            return static_cast<int>(Symbol);
        } else {
            Symbol = ASSIGN;
            token = tmp;
            retract();
            return static_cast<int>(Symbol);
        }
    } else if (isLpar(ch)) {
        Symbol = LPARENT;
        token = "(";
        return static_cast<int>(Symbol);
    } else if (isRpar(ch)) {
        Symbol = RPARENT;
        token = ")";
        return static_cast<int>(Symbol);
    } else if (isLbracket(ch)) {
        Symbol = LBRACK;
        token = "[";
        return static_cast<int>(Symbol);
    } else if (isRbracket(ch)) {
        Symbol = RBRACK;
        token = "]";
        return static_cast<int>(Symbol);
    } else if (isLbrace(ch)) {
        Symbol = LBRACE;
        token = "{";
        return static_cast<int>(Symbol);
    } else if (isRbrace(ch)) {
        Symbol = RBRACE;
        token = "}";
        return static_cast<int>(Symbol);
    }
    return 0;
}

bool isSpace(char in){
    return (in == ' ' || in == '\t' || in == '\r');
}

bool isEnter(char in) {
    return (in == '\n');
}

bool isBlank(char in){
    return (isSpace(in) || isEnter(in));
}

bool isLetter(char in){
    return ((in >= 'A' && in <= 'Z') || (in >= 'a' && in <= 'z') || (in == '_'));
}

bool isDigit(char in){
    return (in >= '0' && in <= '9');
}

bool isColon(char in) {
    return (in == ':');
}

bool isSemicn(char in) {
    return (in == ';');
}

bool isComma(char in) {
    return (in == ',');
}

bool isEqu(char in) {
    return (in == '=');
}

bool isPlus(char in){
    return (in == '+');
}

bool isMinu(char in) {
    return (in == '-');
}

bool isMult(char in){
    return (in == '*');
}
bool isDiv(char in){
    return (in == '/');
}

bool isLss(char in){
    return (in == '<');
}

bool isGre(char in){
    return (in == '>');
}

bool isExcla(char in){
    return (in == '!');
}

bool isLpar(char in) {
    return (in == '(');
}

bool isRpar(char in) {
    return (in == ')');
}

bool isLbracket(char in) {
    return (in == '[');
}

bool isRbracket(char in) {
    return (in == ']');
}

bool isLbrace(char in) {
    return (in == '{');
}

bool isRbrace(char in) {
    return (in == '}');
}

bool isSinglequo(char in){
    return (in == '\'');
}

bool isDoublequo(char in){
    return (in == '\"');
}

bool isChar(char in){
    return (in == 32 || in == 33 || (in >= 35 && in <=126));
}

/**
 * 查找保留字
 * return 如果找到，保留字下标
 * */
int isReserved() {
    for (int i = 0; i < reserved_len; i++) {
        if (inLowerToken.compare(reservedWord[i]) != 0) continue;
        return i;
    }
    return -1;
}

void clearToken() {
    token.clear();
    inLowerToken.clear();
}

/**
 * 连接当前字符到当前token
 */
void catToken() {
    token.append(1, ch);
}

void catLowerToken() {
    inLowerToken.append(1, tolower(ch));
}

bool isEOF() {
    return (indexs >= testfile.length());
}

void retract() {
    indexs--;
}

int transNum() {
    int Num = 0;
    for (int i = 0; i < token.length(); ++i) {
        Num += i * 10 * (token[i] - '0');
    }
    return Num;
}

void getch() {
    ch = testfile[indexs++];
}

void doOutPut() {
    fileout << symbolstring[int(Symbol)] << " " << token << std::endl;
}
