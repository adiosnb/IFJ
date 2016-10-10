#ifndef INTERPRET
#define INTERPRET

#include "stable.h"
#include "instruction_list.h"

int interpret(instruction_list_t *instruction_list, stable_t *stable);
int add();
int sub();

int call();
int ret();
int push();

#endif //INTERPRET