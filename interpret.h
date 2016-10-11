#ifndef INTERPRET
#define INTERPRET

#include "stable.h"
#include "instruction_list.h"

int interpret(instruction_list_t *instruction_list, stab_t *stable);
int add();
int sub();
int write();
int call();
int ret();
int push();
void pop();
void inst_goto();

#endif //INTERPRET