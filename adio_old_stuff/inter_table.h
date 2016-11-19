#ifndef INTER_TABLE
#define INTER_TABLE

#include "instruction_list.h"

#define STAB_SIZE 200;

typedef struct inter_table_elem {
    unsigned tab_key;
    argument_var_t tab_content;
    struct inter_table_elem *tab_next;
} inter_table_elem_t;

typedef struct {
    unsigned tab_size;

    unsigned (*hash_fun_ptr)(unsigned, unsigned);

    unsigned tab_count;
    inter_table_elem_t *arr[];
} inter_table_t;


//alokuje a inicaializuje priestor pre tabulu na 0
inter_table_t *inter_table_init(unsigned size);

//uvolni celu tabulku (tabulka + vsetky prvky)
void inter_table_destroy(inter_table_t **p_table);

//odstrani jeden prvok z tabulky
void inter_table_remove_var(inter_table_t *p_stable, unsigned id);

//prida prvok do tabulky
int inter_table_add_var(inter_table_t *p_stable, unsigned id, argument_var_t *p_var);

//vrati ukazatel na dotazovany prvok
argument_var_t *inter_table_get_var(inter_table_t *p_stable, unsigned id);


#endif //STABLE