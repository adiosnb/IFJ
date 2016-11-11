//
// Created by k on 5.11.2016.
//

#ifndef IFJ_STABLE_H
#define IFJ_STABLE_H

#include <stdbool.h>
#include "instruction_list.h"
#include "instruction_list.h"

typedef struct {
    int type;
    int inter_table;
    argument_var_t data;
}data_t;

typedef struct STAB_ELEMENT{
    char *stab_key;
    data_t stab_content;
    struct STAB_ELEMENT *stab_next;
}stab_element_t;

typedef struct {
    unsigned stab_size;
    unsigned (*hash_fun_ptr)(char *id, unsigned);
    unsigned stab_count;
    stab_element_t *arr[];
}stab_t;

//alokuje a inicaializuje priestor pre tabulu na 0
stab_t *stable_init(unsigned size);

//uvolni celu tabulku (tabulka + vsetky prvky)
void stable_destroy(stab_t **p_table);

//odstrani jeden prvok z tabulky
bool stable_remove_var(stab_t *p_stable, char *id);

//prida prvok do tabulky
int stable_add_var(stab_t *p_stable, char *id, data_t p_var);

//vrati ukazatel na dotazovany prvok
data_t *stable_get_var(stab_t *p_stable, char *id);

//prida prvok zadany oddelene ako 3x string (class, function, local); pri nedostatku miest vracia false
bool stable_add_concatenate(stab_t *p_stable, char* clss, char *fnct, char *local, data_t data);

//true hodnotu ak sa prvok nasiel
bool stable_search(stab_t *p_stable, char *srch_el);

#endif //IFJ_STABLE_H
