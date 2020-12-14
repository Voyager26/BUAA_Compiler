//
// Created by Phantom on 2020/10/10.
//


#include <iostream>
#include <string>
#include <vector>
#include "globalVar.h"
#include "lexical.h"
#include "grammar.h"
#include "genMips.h"
#include "midCode.h"


using namespace std;

int main(){
    /**
     * 输入文件进行处理
     */
    filein.open("testfile.txt", ios::in);
    if (!filein)
        cout << "open file error!" << endl;
    string tmpString;
    while (getline(filein, tmpString)) {
        testfile.append(tmpString);
        testfile.append("\n");
    }
    filein.close();
    //cout << testfile << endl;
    /**
     * 向文件输出
     */
    fileerror.open("error.txt", ios::out);
    fileout.open("output.txt", ios::out);
    mips.open("mips.txt", ios::out);
    midCodefile.open("midCode.txt", ios::out);
    getsym();
    program();
    outputMidCode();
    genMips();
    outputMipsCode();
    mips.close();
    fileerror.close();
    fileout.close();
    midCodefile.close();
    return 0;
}

void error(){
    std::cout << lines << " error!" << std::endl;
}