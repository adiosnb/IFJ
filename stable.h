#ifndef STABLE
#define STABLE

#include "instruction_list.h"

#define STAB_SIZE 200;

typedef struct STAB_ELEMENT{
    unsigned stab_key;
    argument_var_t stab_content;
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
void stable_destroy(stab_t *p_table);
//odstrani jeden prvok z tabulky
void stable_remove_var(unsigned id, stab_t *p_stable);
//prida prvok do tabulky
int stable_add_var(unsigned id, argument_var_t *p_var, stab_t *p_stable);
//vrati ukazatel na dotazovany prvok
argument_var_t *stable_get_var(unsigned id, stab_t *p_stable);





#endif //STABLE