/**************************************
 *   Program :               instruction_list.h
 *   Author :
 *   Login :
 *   Skupina :               2BIB(2016)
 *   Created :               03.10.2016
 *   Compiled:               gcc 4.9.2
 *   Project :               IFJ
 *
 *   Notes :                Instruction list.
 ***************************************/

#ifndef INSTRUCTION_LIST
#define INSTRUCTION_LIST

#define OK              0
#define MALLOC_ERROR    1

typedef struct instruction {
    int type;
    int *addr1;
    int *addr2;
    int *addr3;
} instruction_t;

typedef struct instruction_item  {
    instruction_t   instruction;
    struct instruction_item *next;
} instruction_item_t;

typedef struct instruction_list {
    instruction_item_t *first;
    instruction_item_t *active;
    instruction_item_t *last;
} instruction_list_t;


instruction_list_t *init_inst_list();

int dest_inst_list(instruction_list_t *list);
int add_intstruction(instruction_list_t *i_list,instruction_item_t *instruction);

int create_and_add_instruction(instruction_list_t *list, int type, int *addr1, int *addr2, int *addr3);

#endif //INSTRUCTION_LIST