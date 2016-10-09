#include "interpret.h"
#include "stable.h"
#include "stack.h"
#include "instruction_list.h"

#include <stdlib.h>

stack_t *glob_stack;
stable_t *glob_stable;
instruction_list_t *glob_ins_list;


int interpret(instruction_list_t *instruction_list1, stable_t *stable) {
    glob_stack = stack_init();
    glob_ins_list = instruction_list1;
    glob_stable = stable;



    while(instruction_list1->active != NULL){

        switch (instruction_list1->active->instruction.type) {
            case INST_NOP:
                break;
            case INST_ADD:
                add();
                break;
            case INST_PUSH:
                push();
                break;

        }

        glob_ins_list->active = glob_ins_list->active->next;
    }

    stack_destroy(glob_stack);
    return 0;
}
