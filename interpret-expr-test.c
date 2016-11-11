#include <stdio.h>
#include "stable.h"
#include "interpret.h"


int main () {

    argument_var_t tmp_var;
    argument_var_t *ptr_to_tab;
    instruction_list_t *i_list;
    instruction_item_t *ptr_inst;

    i_list = init_inst_list();
    inter_tab = inter_table_init(1024);





    interpret(i_list, inter_tab);


    dest_inst_list(i_list);
    inter_table_destroy(&inter_tab);

    return 0;
}