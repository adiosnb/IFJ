#ifndef STABLE
#define STABLE

#include "instruction_list.h"

typedef struct STAB_ELEMENT{
    int key;
    argument_var_t cont;
    struct STAB_ELEMENT *next;
}htab_element_t;

typedef struct {
    unsigned size;
    unsigned (*hash_fun_ptr)(int, unsigned);
    unsigned count;
    htab_element_t *arr[];
}stable_t;



argument_var_t *get_var(int id, stable_t *stable);
int add_var(int id, argument_var_t *var,stable_t *stable);




#endif //STABLE