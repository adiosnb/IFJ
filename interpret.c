#include <stdio.h>

#include "interpret.h"
#include "stable.h"
#include "stack.h"
#include "instruction_list.h"

#include <stdlib.h>

stack_t *glob_stack;
stab_t *glob_stable;
instruction_list_t *glob_ins_list;
argument_var_t tmp_var;
argument_var_t *tmp_ptr;

int interpret(instruction_list_t *instruction_list, stab_t *stable) {
    glob_stack = stack_init();
    glob_ins_list = instruction_list;
    glob_stable = stable;

    glob_ins_list->active = glob_ins_list->first;

    while (instruction_list->active != NULL) {

        switch (instruction_list->active->instruction.type) {
            case INST_NOP:
                break;
            case INST_ADD:
                add();
                break;
            case INST_PUSH:
                push();
                break;
            case INST_HALT:
                instruction_list->active = glob_ins_list->last;
                break;
            case INST_CALL:
                //call();
                break;
            case INST_RET:
                //ret();
                break;
            case INST_WRITE:
                tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1,glob_stable);
                write();
                break;


        }

        glob_ins_list->active = glob_ins_list->active->next;
    }

    stack_destroy(&glob_stack);
    return 0;
}

int call() {
    tmp_var.data.instruction = glob_ins_list->active;
    tmp_var.arg_type = INSTRUCTION;
    stack_push(glob_stack, tmp_var);             //pushing current instruction
    tmp_var.arg_type = INTEGER;
    tmp_var.data.i = glob_stack->base;     //pushig base, similar to push ebp in assemlby
    stack_push(glob_stack, tmp_var);
    glob_stack->used += 2;                      //added counter for stack
    glob_stack->base = glob_stack->used;
    tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1, glob_stable);
    glob_ins_list->active = tmp_ptr->data.instruction;
}

int write() {
    switch (tmp_ptr->arg_type) {
        case INTEGER:
            printf("%d",tmp_ptr->data.i);
            break;
        case DOUBLE:
            printf("%g",tmp_ptr->data.d);
            break;
        case STRING:
            printf("%s",tmp_ptr->data.s);
            break;
        case ON_STACK:
            tmp_var = stack_top(glob_stack);
            tmp_ptr = &tmp_var;
            write();
            break;
    }

    return 0;
}

int push(){
    tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1,glob_stable);

    if (tmp_ptr->arg_type == ON_STACK){
        tmp_var = stack_ebp_relative(glob_stack,tmp_ptr->data.i);
        stack_push(glob_stack,tmp_var);
    } else {
        tmp_var = *tmp_ptr;
        stack_push(glob_stack,tmp_var);
    }


    return 0;
}

int add(){


    return 0;
}

