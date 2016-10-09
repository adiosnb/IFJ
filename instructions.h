/**************************************
 *   Program :               instructions.h
 *   Author :
 *   Login :
 *   Skupina :               2BIB(2016)
 *   Created :               03.10.2016
 *   Compiled:               gcc 4.9.2
 *   Project :               IFJ
 *
 *   Notes :                Instruction list.
 ***************************************/
#ifndef INSTRUCTIONS
#define INSTRUCTIONS

//TODO :  v klude dopisujte instrukcie ake potrebujete
enum instructions {
    INST_NOP,
    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIV,
    INST_PUSH,
    INST_POP,
    INST_CALL,
    INST_RET,
    INST_WRITE,
    INST_READ,
};

#endif //INSTRUCTIONS