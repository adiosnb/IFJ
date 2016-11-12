#include <stdio.h>
#include "stable.h"
#include "interpret.h"
#include "instruction_list.h"


int main() {
    stab_t *sym_tab;
    argument_var_t tmp_var;
    argument_var_t *ptr_to_tab;
    instruction_list_t *i_list;
    instruction_item_t *ptr_inst;

    data_t input_data;
    data_t *ptr_to_table1, *ptr_to_table2, *ptr_to_table3;

    sym_tab = stable_init(1024);


    stable_destroy(&sym_tab);
    return 0;
}