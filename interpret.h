#ifndef INTERPRET
#define INTERPRET

#include "stable.h"
#include "instruction_list.h"

int interpret(instruction_list_t *instruction_list, stab_t *stable);

int add();      //DEST OP1 OP2
int sub();      //dest op1 op2
int write();    //src
void call();    //instruction_item_ptr
void ret();     //
int push();     //src
void pop();     //
void inst_jump();//instruction_item_ptr
void jump_not();//instruction_item_ptr  num_fot_condition

#endif //INTERPRET