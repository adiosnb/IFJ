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
#define MINIMAL_MALLOC_SIZE 2048

typedef struct {
    int size;
    int used;
    int base;
    argument_var_t data[];
} stack_t;

stack_t *stack_init();

void stack_destroy(stack_t **stack);

int stack_push(stack_t **stack, argument_var_t var);

argument_var_t stack_pop(stack_t *stack);

argument_var_t stack_top(stack_t *stack);

argument_var_t stack_ebp_relative(stack_t *stack,int position);

argument_var_t *stack_ebp_relative_ptr(stack_t *stack, int position);

int stack_actualize_from_ebp(stack_t *stack, argument_var_t arg, int position);

int resize(stack_t **stack);

void print_stack(stack_t *stack);


#endif //STACK