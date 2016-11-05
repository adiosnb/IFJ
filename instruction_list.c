/**************************************
 *   Program :               instruction_list.c
 *   Author :
 *   Login :
 *   Skupina :               2BIB(2016)
 *   Created :               03.10.2016
 *   Compiled:               gcc 4.9.2
 *   Project :               IFJ
 *
 *   Notes :                Instruction list.
 ***************************************/

#include "instruction_list.h"
#include "instructions.h"

#include <stdlib.h>
#include <stdio.h>


instruction_list_t *init_inst_list() {
    instruction_list_t *new_list;
    instruction_item_t *new_instruction;

    if((new_list = malloc(sizeof(instruction_list_t))) == NULL) {
        return NULL;
    }

    //vytvorenie prazdnej instrukcie pre odstranenie podmienky v create and add instruction
    if((new_instruction = malloc(sizeof(instruction_item_t))) == NULL) {
        free(new_list);        //deleting list if second malloc fails
        return NULL;
    }

    new_instruction->instruction.addr1 = 0;
    new_instruction->instruction.addr2 = 0;
    new_instruction->instruction.addr3 = 0;
    new_instruction->instruction.type = INST_NOP;
    new_instruction->next = NULL;

    new_list->active = NULL;
    new_list->first  = new_instruction;
    new_list->last   = new_instruction;

    return new_list;
}


int dest_inst_list(instruction_list_t *list) {
    instruction_item_t *current_item, *next_item;

    current_item = list->first;
    while (current_item != NULL) {          //cycle through all items in list
        next_item = current_item->next;
        free(current_item);                //deleting of each item of list
        current_item = next_item;
    }

    free(list);                            //deleting of whole lsit

    return OK;
}


instruction_item_t *create_and_add_instruction(instruction_list_t *list, enum instructions type, unsigned addr1,
                                               unsigned addr2, unsigned addr3) {

    instruction_item_t *new_instruction;

    if ((new_instruction = malloc(sizeof(instruction_item_t))) == NULL) {
        return NULL;
    }

    new_instruction->instruction.addr1 = addr1;
    new_instruction->instruction.addr2 = addr2;
    new_instruction->instruction.addr3 = addr3;
    new_instruction->instruction.type  = type;
    new_instruction->next              = NULL;

    list->last->next = new_instruction;
    list->last = new_instruction;

    return new_instruction;
}

void debug(instruction_list_t* list){

    char *msg[] = {
            "INST_NOP",
            "INST_ADD",
            "INST_SUB",
            "INST_MUL",
            "INST_DIV",
            "INST_PUSH",
            "INST_POP",
            "INST_STORE",
            "INST_JMP",
            "INST_JEQ",
            "INST_JNEQ",
            "INST_JZ",
            "INST_CALL",
            "INST_RET",
            "INST_WRITE",
            "INST_READ_INT",
            "INST_READ_DOUBLE",
            "INST_READ_STRING",
            "INST_STR_LEN",
            "INST_STR_CMP",
            "INST_STR_FIND",
            "INST_STR_SORT",
            "INST_STR_SUBSTR",
            "INST_HALT",
            "INST_LABEL",
            NULL,
    };

    printf(        "****************************\n"
                   "* %i                       *\n"
                   "* %i                       *\n"
                   "* %i                       *\n"
                   "* %s                       *\n",
                   list->active->instruction.addr1,
                   list->active->instruction.addr2,
                   list->active->instruction.addr3,
                   msg[list->active->instruction.type]);

}