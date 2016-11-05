//
// Created by k on 5.11.2016.
//

#ifndef IFJ_STABLE_H
#define IFJ_STABLE_H

#include "instruction_list.h"

typedef struct {
    int type;
    int inter_table;
}data_t;

typedef struct STAB_ELEMENT{
    char *stab_key;
    data_t stab_content;
    struct STAB_ELEMENT *stab_next;
}stab_element_t;

typedef struct {
    unsigned stab_size;
    unsigned (*hash_fun_ptr)(unsigned, unsigned);
    unsigned stab_count;
    stab_element_t *arr[];
}stab_t;

//alokuje a inicaializuje priestor pre tabulu na 0
stab_t *stable_init(unsigned size);

//uvolni celu tabulku (tabulka + vsetky prvky)
void stable_destroy(stab_t **p_table);

//odstrani jeden prvok z tabulky
void stable_remove_var(stab_t *p_stable, unsigned id);

//prida prvok do tabulky
int stable_add_var(stab_t *p_stable, unsigned id, argument_var_t *p_var);

//vrati ukazatel na dotazovany prvok
argument_var_t *stable_get_var(stab_t *p_stable, unsigned id);

#endif //IFJ_STABLE_H
