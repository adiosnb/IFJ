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
    INST_SUB,           // destination      operand1        operand2
    INST_MUL,           // destination      operand1        operand2
    INST_DIV,           // destination      operand1        operand2
    INST_PUSH,          //var to push       ---             ---
    INST_POP,           //no inputs
    INST_STORE,         //TODO
    INST_JMP,           //addr to jump      ---             ---
    INST_JEQ,           //ddr to jump       value           value
    INST_JNEQ,          //ddr to jump       value           value
    INST_JZ,            //addr to jump      value for equation  ---
    INST_CALL,          //addr of func      where to save return value or nothing     ---
    INST_RET,           //value for return  ---             ---
    INST_WRITE,         //source            ---             ---
    INST_READ_INT,      //destination       ---             ---
    INST_READ_DOUBLE,   //destination       ---             ---
    INST_READ_STRING,       //TODO
    INST_STR_LEN,       //TODO
    INST_STR_CMP,       //TODO
    INST_STR_FIND,      //TODO
    INST_STR_SORT,      //TODO
    INST_STR_SUBSTR,    //TODO
    INST_HALT,          //no imputs
    INST_LABEL,         //no inputs
};

#endif //INSTRUCTIONS