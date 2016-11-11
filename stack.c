/**************************************
 *   Program :               stack.c
 *   Author :
 *   Login :
 *   Skupina :               2BIB(2016)
 *   Created :               03.10.2016
 *   Compiled:               gcc 4.9.2
 *   Project :               IFJ
 *
 *   Notes :                Stack
 ***************************************/
#include "stack.h"

#include <stdlib.h>
#include <stdio.h>


stack_t *stack_init() {
    stack_t *stack;

    if ((stack = malloc(sizeof(stack_t) + sizeof(argument_var_t)*MINIMAL_MALLOC_SIZE)) == NULL){
        return NULL;
    }


    stack->size = MINIMAL_MALLOC_SIZE;
    stack->used = 0;
    stack->base = 0;

    return stack;
}

void stack_destroy(stack_t **stack){
    free(*stack);
}

int stack_push(stack_t **stack, argument_var_t var) {
    if (stack[0]->size <= stack[0]->used){
        resize(stack);
    }

    stack[0]->data[stack[0]->used] = var;

    stack[0]->used++;
    return 0;
}

argument_var_t stack_pop(stack_t *stack) {
    argument_var_t pop_var;
    if(stack->used > 0) {
        pop_var = stack->data[stack->used - 1];
        stack->used--;
    } else pop_var.arg_type = -1;

    return pop_var;
}


argument_var_t stack_top(stack_t *stack){
    return stack->data[stack->used - 1];    //-1 je tam preto ze nepouzivam premennu stack.top ale stack.used
}


argument_var_t stack_ebp_relative(stack_t *stack, int position){
    return stack->data[stack->base + position - 1];
}

argument_var_t *stack_ebp_relative_ptr(stack_t *stack, int position){
    return &stack->data[stack->base + position - 1];
}


int stack_actualize_from_ebp(stack_t *stack, argument_var_t arg, int position){
    stack->data[stack->base + position - 1] = arg;
    return 0;
}


void print_stack(stack_t *stack) {
    printf("\n---------------------------------\n");
    for (int i = 0; i < stack->used; i ++){
        printf("%d : %d\n",i,stack->data[i].data.i);
    }
    printf("\n---------------------------------\n");
}

void resize(stack_t **stack) {
    *stack = realloc(*stack,sizeof(stack_t) + sizeof(argument_var_t)*(stack[0]->used + MINIMAL_MALLOC_SIZE));
    stack[0]->size += MINIMAL_MALLOC_SIZE;
}