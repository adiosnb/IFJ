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
                printf("CALL\n");
                call();
                break;
            case INST_RET:
                ret();
                printf("RET\n");
                break;
            case INST_JMP:
                inst_jump();
                break;
            case INST_WRITE:
                tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1,glob_stable);
                write();
                printf("\n");
                break;
            case INST_JNP:
                jump_not();
                break;


        }
        //print_stack(glob_stack);
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
    int prev_base;

    tmp_var = stack_ebp_relative(glob_stack, 0);
    prev_base = tmp_var.data.i;
    tmp_var = stack_ebp_relative(glob_stack, -1);
    glob_ins_list->active = tmp_var.data.instruction;
    glob_stack->used = glob_stack->base;
    glob_stack->base = prev_base;
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

    if (tmp_ptr->arg_type == STACK_EBP) {
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

    if (arg1->arg_type == STACK_EBP){
        num1 = stack_ebp_relative(glob_stack,arg1->data.i);
    } else {
        num1 = *arg1;
    }

    if (arg2->arg_type == STACK_EBP){
        num2 = stack_ebp_relative(glob_stack,arg2->data.i);
    } else {
        num1 = *arg2;
    }

    if (arg3->arg_type == STACK_EBP){
        num3 = stack_ebp_relative(glob_stack,arg3->data.i);
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
    stack_pop(glob_stack);
}

void inst_jump() {
    tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1,
                             glob_stable);     //nacita z tabulky symbolov ukazatel na instrukciu
    glob_ins_list->active = tmp_ptr->data.instruction;                                  //priradi ukazatel do listu, takze zmeni tok programu
}

void jump_not() {
    tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr2, glob_stable);     //nacita operand pre porovnanie
    if (tmp_ptr->arg_type == STACK_EBP) {
        tmp_var = stack_ebp_relative(glob_stack,
                                     tmp_ptr->data.i);                       //ak je na zasobniku zoberie hodnotu operandu z tade
    } else {
        tmp_var = *tmp_ptr;
    }

    if (tmp_var.data.i ==
        0) {                                                            //ak je operand nulovy takze false, urobi sa skok
        tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1,
                                 glob_stable);  //nacita z tabulky symbolov ukazatel na instrukciu
        glob_ins_list->active = tmp_ptr->data.instruction;                               //priradi ukazatel do listu, takze zmeni tok programu
    }
}

