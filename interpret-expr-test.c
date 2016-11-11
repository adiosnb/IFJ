#include <stdio.h>
#include "stable.h"
#include "interpret.h"


int main () {
    stab_t *sym_tab;
    argument_var_t tmp_var;
    argument_var_t *ptr_to_tab;
    instruction_list_t *i_list;
    instruction_item_t *ptr_inst;

    data_t input_data;
    data_t *ptr_to_table1, *ptr_to_table2, *ptr_to_table3;

    i_list = init_inst_list();
    sym_tab = stable_init(2);

    char lokal_a[] = "Main.Run.a";
    char lokal_b[] = "Main.Run.b";
    char lokal_c[] = "Main.Run.c";
    char stack_top[] = "stack-top";


    input_data.data.arg_type = INTEGER;
    input_data.data.data.i = 5;
    stable_add_var(sym_tab, lokal_a, input_data);

    input_data.data.arg_type = INTEGER;
    input_data.data.data.i = 8;
    stable_add_var(sym_tab, lokal_b, input_data);

    input_data.data.arg_type = STACK_EBP;
    input_data.data.data.i = 1;
    stable_add_var(sym_tab, lokal_c, input_data);

    input_data.data.arg_type = ON_TOP;
    input_data.data.data.i = 1;
    stable_add_var(sym_tab, stack_top, input_data);


    create_and_add_instruction(i_list, INST_PUSH_INT, 0, 0, 0);
    ptr_to_table1 = stable_get_var(sym_tab, lokal_a);
    create_and_add_instruction(i_list, INST_PUSH, &ptr_to_table1->data, 0, 0);
    ptr_to_table1 = stable_get_var(sym_tab, lokal_b);
    create_and_add_instruction(i_list, INST_PUSH, &ptr_to_table1->data, 0, 0);
    create_and_add_instruction(i_list, INST_EXPR_ADD, 0, 0, 0);

    ptr_to_table1 = stable_get_var(sym_tab, lokal_c);
    ptr_to_table2 = stable_get_var(sym_tab, stack_top);
    create_and_add_instruction(i_list, INST_STORE, &ptr_to_table1->data, &ptr_to_table2->data, 0);
    create_and_add_instruction(i_list, INST_POP, 0, 0, 0);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table1->data, 0, 0);
    create_and_add_instruction(i_list, INST_HALT, 0, 0, 0);


    interpret(i_list, sym_tab);
    dest_inst_list(i_list);
    stable_destroy(&sym_tab);


    return 0;
}