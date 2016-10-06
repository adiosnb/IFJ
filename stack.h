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
#define MINIMAL_MALLOC_SIZE 1024

typedef struct {
    int size;
    int used;
    int base;
    variable_t *data;
} stack_t;

stack_t *stack_init();

void stack_destroy(stack_t *stack);

int stack_push(stack_t *stack,variable_t var);

variable_t stack_pop(stack_t *stack);

void stack_get_value();

int resize(stack_t *stack);


#endif //STACK