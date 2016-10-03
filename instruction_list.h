#ifndef INSTRUCTION_LIST
#define INSTRUCTION_LIST

typedef struct instruction {
    int type;
    int *addr1;
    int *addr2;
    int *addr3;
} instruction_t;

typedef struct instruction_item  {
    instruction_t   instruction;
    instruction_t   *next;
} instruction_item_t;

typedef struct instruction_list {
    instruction_item_t *first;
    instruction_item_t *active;
    instruction_item_t *last;
} instruction_list_t;


instruction_list_t *init();
int add_intstruction(instruction_list_t *i_list,instruction_item_t *instruction);
int create_and_add_instruction(instruction_list_t *list, int type, int addr1, int addr2, int addr3);

#endif //INSTRUCTION_LIST