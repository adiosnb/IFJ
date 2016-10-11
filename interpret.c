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
            case INST_POP:
                pop();
                break;
            case INST_HALT:
                instruction_list->active = glob_ins_list->last;
                break;
            case INST_CALL:
                call();
                break;
            case INST_RET:
                ret();
            case INST_GOTO:
                inst_goto();
                break;
            case INST_WRITE:
                tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1,glob_stable);
                write();
                break;


        }
        print_stack(glob_stack);
        glob_ins_list->active = glob_ins_list->active->next;
    }

    stack_destroy(&glob_stack);
    return 0;
}

void call() {
    tmp_var.data.instruction = glob_ins_list->active;
    tmp_var.arg_type = INSTRUCTION;
    stack_push(glob_stack, tmp_var);             //pushing current instruction
    tmp_var.arg_type = INTEGER;
    tmp_var.data.i = glob_stack->base;     //pushig base, similar to push ebp in assemlby
    stack_push(glob_stack, tmp_var);
    //glob_stack->used += 2;                      //added counter for stack
    glob_stack->base = glob_stack->used;
    tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1, glob_stable);
    glob_ins_list->active = tmp_ptr->data.instruction;
}

void ret(){
    tmp_var = stack_pop(glob_stack);
    glob_stack->base = tmp_var.data.i;
    tmp_var = stack_pop(glob_stack);
    glob_ins_list->active = tmp_var.data.instruction;
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
        case STACK_EBP:
            tmp_var = stack_ebp_relative(glob_stack,tmp_ptr->data.i);
            tmp_ptr = &tmp_var;
            write();
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
    argument_var_t  *arg1,*arg2,*arg3;

    argument_var_t num1,num2,num3;

    arg1 = stable_get_var(glob_ins_list->active->instruction.addr1,glob_stable);
    arg2 = stable_get_var(glob_ins_list->active->instruction.addr2,glob_stable);
    arg3 = stable_get_var(glob_ins_list->active->instruction.addr3,glob_stable);

    double a,b;

    if (arg1->arg_type == ON_STACK || arg1->arg_type == STACK_EBP){
        if (arg1->arg_type == ON_STACK){
            //TODO
        } else {
            num1 = stack_ebp_relative(glob_stack,arg1->data.i);
        }
    } else {
        num1 = *arg1;
    }

    if (arg2->arg_type == ON_STACK || arg2->arg_type == STACK_EBP){
        if (arg2->arg_type == ON_STACK){
            //TODO
        } else {
            num2 = stack_ebp_relative(glob_stack,arg2->data.i);
        }
    } else {
        num1 = *arg2;
    }

    if (arg3->arg_type == ON_STACK || arg3->arg_type == STACK_EBP){
        if (arg3->arg_type == ON_STACK){
            //TODO
        } else {
            num3 = stack_ebp_relative(glob_stack,arg3->data.i);
        }
    } else {
        num1 = *arg3;
    }

    if (num2.arg_type == DOUBLE){
        a = num2.data.d;
    } else {
        a = num2.data.i;
    }

    if (num3.arg_type == DOUBLE){
        b = num3.data.d;
    } else {
        b = num3.data.i;
    }

    if (num1.arg_type == DOUBLE){
        num1.data.d = a + b;
    } else {
        num1.data.i = (int) (a + b);
    }

    if (arg1->arg_type == STACK_EBP) {
        stack_actualize_from_ebp(glob_stack, num1, arg1->data.i);
    } else {
        *arg1 = num1;
    }

    return 0;
}


void pop(){
    //glob_stack->used--;
    stack_pop(glob_stack);
}

void inst_goto() {
    tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1,glob_stable);

    if (tmp_ptr->arg_type == INSTRUCTION){
        glob_ins_list->active = tmp_ptr->data.instruction;
    } else {
        //TODO : load instruction from stack
    }
}

