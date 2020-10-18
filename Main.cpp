//
// Created by Phantom on 2020/10/10.
//


#include <iostream>
#include <string>
#include <vector>
#include "globalVar.h"
#include "lexical.h"
#include "grammar.h"



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
    fileout.open("output.txt", ios::out);
    getsym();
    program();
    fileout.close();
    return 0;
}

void error(){
    std::cout << "error!" << std::endl;
}