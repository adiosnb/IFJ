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
enum instructions {     // ADDR1            ADDR2           ADDR3
    INST_NOP,           //no inputs
    INST_ADD,           // destination      operand1        operand2
    INST_SUB,           //TODO
    INST_MUL,           //TODO
    INST_DIV,           //TODO
    INST_PUSH,          //var to push       ---             ---
    INST_POP,           //no inputs
    INST_STORE,         //TODO
    INST_JMP,           //addr to jump      ---             ---
    INST_JNP,           //addr to jump      value for equation  ---
    INST_CALL,          //addr of func      where to save return value or nothing     ---
    INST_RET,           //no inputs
    INST_WRITE,         //source            ---             ---
    INST_READ_INT,          //TODO
    INST_READ_DOUBLE,       //TODO
    INST_READ_STRING,       //TODO
    INST_HALT,          //no imputs
    INST_LABEL,         //no inputs
};

#endif //INSTRUCTIONS