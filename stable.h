#ifndef STABLE
#define STABLE

#include "instruction_list.h"

argument_var_t *get_var(int id, stable_t *stable);
int add_var_to_table(int id, argument_var_t *var,stable_t *stable);




#endif //STABLE