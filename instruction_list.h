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

#include "instructions.h"


//return values from functions
#define OK              0
#define MALLOC_ERROR    1

//variable type
#define ERROR           -1
#define INTEGER         0
#define DOUBLE          1
#define STRING          2
#define ON_STACK        3
#define STACK_EBP       4
#define INSTRUCTION     5


typedef struct {
    enum instructions type;
    unsigned addr1;
    unsigned addr2;
    unsigned addr3;
} instruction_t;

typedef struct instruction_item  {
    instruction_t   instruction;
    struct instruction_item *next;
} instruction_item_t;

typedef struct {
    instruction_item_t *first;
    instruction_item_t *active;
    instruction_item_t *last;
} instruction_list_t;

typedef union {
    int i;
    double d;
    char *s;
    instruction_item_t *instruction;
} var_u;

typedef struct {
    int arg_type;
    var_u data;
} argument_var_t;


instruction_list_t *init_inst_list();

int dest_inst_list(instruction_list_t *list);
int add_intstruction(instruction_list_t *i_list,instruction_item_t *instruction);

instruction_item_t *create_and_add_instruction(instruction_list_t *list, enum instructions type, unsigned addr1,
                                               unsigned addr2, unsigned addr3);



#endif //INSTRUCTION_LIST