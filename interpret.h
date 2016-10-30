#ifndef INTERPRET
#define INTERPRET

#include "stable.h"
#include "instruction_list.h"

int interpret(instruction_list_t *instruction_list, stab_t *stable);

void add();      //DEST OP1 OP2
void sub();      //dest op1 op2
void write();    //src
void read_int();//dst
void read_double();//dst
void read_string();//
void call();    //instruction_item_ptr
void ret();     //
int push();     //src
void pop();     //
void inst_jump();//instruction_item_ptr
void jump_not();//instruction_item_ptr  num_fot_condition

#endif //INTERPRET