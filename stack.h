/**************************************
 *   Program :               stack.h
 *   Author :
 *   Login :
 *   Skupina :               2BIB(2016)
 *   Created :               03.10.2016
 *   Compiled:               gcc 4.9.2
 *   Project :               IFJ
 *
 *   Notes :                Stack
 ***************************************/
#ifndef STACK
#define STACK

#include "instruction_list.h"

//minimalna velkost pre alokaciu
#define MINIMALL_MALLOC_SIZE 1024

typedef struct stack {
    int size;
    int used;
    variable_t *data;
} stack_t;

stack_t *init_stack();

void destroy_stack(stack_t *stack);

int push();

int pop();

void get_value();

int resize();


#endif //STACK