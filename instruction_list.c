#include "instruction_list.h"

#include <stdlib.h>


instruction_list_t *init() {
    instruction_list_t *new_list;
    instruction_item_t *new_instruction;

    if((new_list = malloc(sizeof(instruction_list_t))) == NULL) {
        return NULL;
    }

    //vytvorenie prazdnej instrukcie pre odstranenie podmienky v create and add instruction
    if((new_instruction = malloc(sizeof(instruction_item_t))) == NULL) {
        free(&new_list);
        return NULL;
    }

    new_list->active = NULL;
    new_list->first  = new_instruction;
    new_list->last   = new_instruction;

    return new_list;
}


int create_and_add_instruction(instruction_list_t *list, int type, int addr1, int addr2, int addr3) {
    instruction_item_t *new_instruction;
    if ((new_instruction = malloc(sizeof(instruction_item_t))) == NULL) {
        return 1;
        //TODO treba dorobit error
    }

    new_instruction->instruction.addr1 = addr1;
    new_instruction->instruction.addr2 = addr2;
    new_instruction->instruction.addr3 = addr3;
    new_instruction->instruction.type  = type;
    new_instruction->next              = NULL;
//TODO treba to skontrolovat
    list->last->next = new_instruction;
    list->last = new_instruction;

    return 0;
}