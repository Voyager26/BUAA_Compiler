//
// Created by Phantom on 2020/11/14.
//

#include "midCode.h"
#include "globalVar.h"
void outputMidCode() {
    for (int i = 0; i < midCodeTable.size(); i++) {
        midCode mc = midCodeTable[i];
        switch (mc.op) {
            case MainOp:
                midCodefile << "---main在这里---" << "\n";
                break;
            case PlusOp:
                midCodefile << mc.z << " = " << mc.x << " + " << mc.y << "\n";
                break;
            case MinuOp:
                midCodefile << mc.z << " = " << mc.x << " - " << mc.y << "\n";
                break;
            case MultOp:
                midCodefile << mc.z << " = " << mc.x << " * " << mc.y << "\n";
                break;
            case DivOp:
                midCodefile << mc.z << " = " << mc.x << " / " << mc.y << "\n";
                break;
            case LssOp:
                midCodefile  << mc.x << " >= " << mc.y << " goto "<< mc.z << "\n";
                break;
            case LeqOp:
                midCodefile << mc.x << " > " << mc.y << " goto "<< mc.z << "\n";
                break;
            case GreOp:
                midCodefile << mc.x << " < " << mc.y << " goto "<< mc.z << "\n";
                break;
            case GeqOp:
                midCodefile << mc.x << " <= " << mc.y << " goto "<< mc.z << "\n";
                break;
            case BeqOp:
                midCodefile << mc.x << " != " << mc.y << " goto "<< mc.z << "\n";
                break;
            case NeqOp:
                midCodefile << mc.x << " == " << mc.y << " goto "<< mc.z << "\n";
                break;
            case AssignOp:
                midCodefile << mc.z << " = " << mc.x << "\n";
                break;
            case GoTo:
                midCodefile << "GOTO " << mc.z << "\n";
                break;
            case BZ:
                midCodefile << "BZ " << mc.z << "(" << mc.x << "=0)" << "\n";
                break;
            case BNZ:
                midCodefile << "BNZ " << mc.z << "(" << mc.x << "=1)" << "\n";
                break;
            case PushOp:
                midCodefile << "PUSH " << mc.z << "\n";
                break;
            case Call:
                midCodefile << "CALL " << mc.z << "\n";
                break;
            case ReturnOp:
                midCodefile << "RET " << mc.z << "\n";
                break;
            case ReturnValue:
                midCodefile << "RETVALUE " << mc.z << " = " << mc.x << "\n";
                break;
            case ScanOp:
                midCodefile << "SCAN " << mc.z << "\n";
                break;
            case PrintOp:
                midCodefile << "PRINT " << mc.z << " " << mc.x << "\n";
                break;
            case Label:
                midCodefile << mc.z << ": \n";
                break;
            case ArrayOp:
                midCodefile << "ARRAY " << mc.z << " " << mc.x << " 是否初始化" << mc.y << endl;
                break;
            case VarOp:
                midCodefile << "VAR " << mc.z << " " << mc.x << endl;
                break;//*/
            case FuncOp:
                midCodefile << "FUNC " << mc.z << " " << mc.x << "()" << endl;
                break;
            case ParamOp:
                midCodefile << "PARA " << mc.z << " " << mc.x << endl;
                break;
            case GetArray:
                midCodefile << mc.z << " = " << mc.x << " 维度" << mc.y << "\n";
                break;
            case PutArray:
                midCodefile << mc.x << " 维度" << mc.y  << " = " << mc.z << "\n";
                break;
            case Exit:
                midCodefile << "EXIT programme\n";
                break;
            default:
                break;
        }
    }
}