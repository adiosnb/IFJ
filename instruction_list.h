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

//variable types
#define INTEGER         0
#define DOUBLE          1
#define STRING          2

//variables for type of data
#define ON_STACK        0
#define IN_UNION        1

typedef union {
    int i;
    double d;
    char *s;
} var_u;

typedef struct variable {
    int data_type;
    var_u data;
} variable_t;

typedef struct argument_var {
    int arg_type;
    int position_on_stack;
    variable_t data;
} argument_var_t;

typedef struct {
    enum instructions type;
    int addr1;
    int addr2;
    int addr3;
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

int create_and_add_instruction(instruction_list_t *list, enum instructions type, int addr1, int addr2, int addr3);

#endif //INSTRUCTION_LIST