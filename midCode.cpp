//
// Created by Phantom on 2020/11/14.
//

#include "midCode.h"
#include "globalVar.h"
void outputMidCode() {
    for (int i = 0; i < midCodeTable.size(); i++) {
        midCode mc = midCodeTable[i];
        switch (mc.op) {
            case Main:
                midCodefile << "--- here is main---" << "\n";
                break;
            case PLUSOP:
                midCodefile << mc.z << " = " << mc.x << " + " << mc.y << "\n";
                break;
            case MINUOP:
                midCodefile << mc.z << " = " << mc.x << " - " << mc.y << "\n";
                break;
            case MULTOP:
                midCodefile << mc.z << " = " << mc.x << " * " << mc.y << "\n";
                break;
            case DIVOP:
                midCodefile << mc.z << " = " << mc.x << " / " << mc.y << "\n";
                break;
            case LSSOP:
                midCodefile  << mc.x << " >= " << mc.y << " goto "<< mc.z << "\n";
                break;
            case LEQOP:
                midCodefile << mc.x << " > " << mc.y << " goto "<< mc.z << "\n";
                break;
            case GREOP:
                midCodefile << mc.x << " < " << mc.y << " goto "<< mc.z << "\n";
                break;
            case GEQOP:
                midCodefile << mc.x << " <= " << mc.y << " goto "<< mc.z << "\n";
                break;
            case BEQOP:
                midCodefile << mc.x << " != " << mc.y << " goto "<< mc.z << "\n";
                break;
            case NEQOP:
                midCodefile << mc.x << " == " << mc.y << " goto "<< mc.z << "\n";
                break;
            case ASSIGNOP:
                midCodefile << mc.z << " = " << mc.x << "\n";
                break;
            case GOTO:
                midCodefile << "GOTO " << mc.z << "\n";
                break;
            case BZ:
                midCodefile << "BZ " << mc.z << "(" << mc.x << "=0)" << "\n";
                break;
            case BNZ:
                midCodefile << "BNZ " << mc.z << "(" << mc.x << "=1)" << "\n";
                break;
            case PUSH:
                midCodefile << "PUSH " << mc.z << "\n";
                break;
            case CALL:
                midCodefile << "CALL " << mc.z << "\n";
                break;
            case RET:
                midCodefile << "RET " << mc.z << "\n";
                break;
            case RETVALUE:
                midCodefile << "RETVALUE " << mc.z << " = " << mc.x << "\n";
                break;
            case SCAN:
                midCodefile << "SCAN " << mc.z << "\n";
                break;
            case PRINT:
                midCodefile << "PRINT " << mc.z << " " << mc.x << "\n";
                break;
            case LABEL:
                midCodefile << mc.z << ": \n";
                break;
            case ARRAY:
                midCodefile << "ARRAY " << mc.z << " " << mc.x << " 是否初始化" << mc.y << endl;
                break;
            case VAR:
                midCodefile << "VAR " << mc.z << " " << mc.x << endl;
                break;//*/
            case FUNC:
                midCodefile << "FUNC " << mc.z << " " << mc.x << "()" << endl;
                break;
            case PARAM:
                midCodefile << "PARA " << mc.z << " " << mc.x << endl;
                break;
            case GETARRAY:
                midCodefile << mc.z << " = " << mc.x << " 维度" << mc.y << "\n";
                break;
            case PUTARRAY:
                midCodefile << mc.x << " 维度" << mc.y  << " = " << mc.z << "\n";
                break;
            case EXIT:
                midCodefile << "EXIT\n";
                break;
            default:
                break;
        }
    }
}
