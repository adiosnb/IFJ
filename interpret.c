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


    while (instruction_list->active != NULL) {

        switch (instruction_list->active->instruction.type) {
            case INST_NOP:
                break;
            case INST_ADD:
                //add();
                break;
            case INST_PUSH:
                //push();
                break;
            case INST_HALT:
                //instruction_list->active = NULL;
                break;
            case INST_CALL:
                //call();
                break;
            case INST_RET:
                //ret();
                break;



        }

        glob_ins_list->active = glob_ins_list->active->next;
    }

    stack_destroy(glob_stack);
    return 0;
}

int call() {
    tmp_var.instruction = glob_ins_list->active;
    tmp_var.arg_type = INSTRUCTION;
    stack_push(glob_stack, tmp_var);             //pushing current instruction
    tmp_var.arg_type = IN_UNION;
    tmp_var.data.data_type = INTEGER;
    tmp_var.data.data.i = glob_stack->base;     //pushig base, similar to push ebp in assemlby
    stack_push(glob_stack, tmp_var);
    glob_stack->used += 2;                      //added counter for stack
    glob_stack->base = glob_stack->used;
    tmp_ptr = stable_get_var(glob_ins_list->active->instruction.addr1, glob_stable);
    glob_ins_list->active = tmp_ptr->instruction;
}