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


stack_t *stack_init() {
    stack_t *stack;

    if ((stack = malloc(sizeof(stack_t))) == NULL){
        return NULL;
    }

    if ((stack->data = malloc(sizeof(variable_t) * MINIMAL_MALLOC_SIZE)) == NULL) {
        free(&stack);
        return NULL;
    }

    stack->size = MINIMAL_MALLOC_SIZE;
    stack->used = 0;
    stack->base = 0;

    return stack;
}

void stack_destroy(stack_t *stack){
    free(&stack->data);
    free(&stack);
}

int stack_push(stack_t *stack,variable_t var){
    if (stack->size <= stack->used){
        resize(stack);
    }

    //stack->data[stack->used].data_type = var.data_type;
    stack->data[stack->used] = var; //TODO treba overit ci to je spravne, ak nie rozpisat to do ifov alebo switch

    stack->used++;
    return 0;
}

variable_t stack_pop(stack_t *stack){
    variable_t pop_var;
    if(stack->used > 0) {
        pop_var = stack->data[stack->used]; //TODO treba overit ci to je spravne, ak nie rozpisat to do ifov alebo switch
        stack->used--;
    } else pop_var.data_type = -1;

    return pop_var;
}


