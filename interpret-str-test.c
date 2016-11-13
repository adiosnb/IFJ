#include <stdio.h>
#include "stable.h"
#include "interpret.h"
#include "instruction_list.h"

int main() {
    stab_t *sym_tab;
    data_t tmp_var;
    instruction_list_t *i_list;

    char str1[] = "qwertyuiop";

    data_t *ptr_to_table1, *ptr_to_table2, *ptr_to_table3;
/*

    printf("Test nad string operaciami v globalnych premennych\n"
                   "v str1 je ulozeny retazec\n"
                   "do str2 sa pomocou str2 nahra retazec zo stdin\n"
                   "do str3 sa ulozi ich konkatenacia\n"
                   "do int a sa ulozi dlzka str2\n"
                   "do int b sa ulozi dlzka str3\n\n");

    sym_tab = stable_init(1024);

    tmp_var.data.arg_type = STRING;
    tmp_var.data.data.s = str_init();
    str_append_chars(&tmp_var.data.data.s, str1);
    stable_add_var(sym_tab, "glob_str_1", tmp_var);
    tmp_var.data.data.s = str_init();
    stable_add_var(sym_tab, "glob_str_2", tmp_var);
    tmp_var.data.data.s = str_init();
    stable_add_var(sym_tab, "glob_str_3", tmp_var);
    tmp_var.data.arg_type = INTEGER;
    stable_add_var(sym_tab, "INT.A", tmp_var);
    stable_add_var(sym_tab, "INT.B", tmp_var);

    //stable_print(sym_tab);

    i_list = init_inst_list();

    ptr_to_table1 = stable_get_var(sym_tab, "glob_str_1");
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table1->data, 0, 0);
    ptr_to_table1 = stable_get_var(sym_tab, "glob_str_2");
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table1->data, 0, 0);
    ptr_to_table1 = stable_get_var(sym_tab, "glob_str_2");
    create_and_add_instruction(i_list, INST_READ_STRING, &ptr_to_table1->data, 0, 0);
    ptr_to_table1 = stable_get_var(sym_tab, "glob_str_2");
    ptr_to_table2 = stable_get_var(sym_tab, "INT.A");
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table1->data, 0, 0);
    create_and_add_instruction(i_list, INST_STR_LEN, &ptr_to_table2->data, &ptr_to_table1->data, 0);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table2->data, 0, 0);
    ptr_to_table1 = stable_get_var(sym_tab, "glob_str_1");
    ptr_to_table2 = stable_get_var(sym_tab, "glob_str_2");
    ptr_to_table3 = stable_get_var(sym_tab, "glob_str_3");

    create_and_add_instruction(i_list, INST_STR_CONCATENATE, &ptr_to_table3->data, &ptr_to_table2->data,
                               &ptr_to_table1->data);

    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table3->data, 0, 0);
    ptr_to_table2 = stable_get_var(sym_tab, "INT.B");
    create_and_add_instruction(i_list, INST_STR_LEN, &ptr_to_table2->data, &ptr_to_table3->data, 0);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table2->data, 0, 0);

    create_and_add_instruction(i_list, INST_HALT, 0, 0, 0);

    interpret(i_list, sym_tab);
    //stable_print(sym_tab);

    dest_inst_list(&i_list);
    stable_destroy(&sym_tab);

    printf("\n\n"
                   "TEST2\n"
                   "praca so stringami na zasobniku\n"
                   "vytvorime 3 lokalne stringy {str1,str2,str3}\n"
                   "do prvych 2 nacitame hodnotu pomocou read_string\n"
                   "a do str3 ulozime ich konkatenaciu\n\n"
                   "Umiestnenie na stacku:\n"
                   "-----------------------------\n"
                   "|str1                      1|\n"
                   "|str2                      2|\n"
                   "|str3                      3|\n"
                   "|len1                      4|\n"
                   "|len2                      5|\n"
                   "|l2n3                      6|\n"
                   "-----------------------------\n"
    );

    i_list = init_inst_list();
    sym_tab = stable_init(10);

    tmp_var.data.arg_type = STACK_EBP;
    tmp_var.data.data.i = 1;
    stable_add_var(sym_tab, "str_1", tmp_var);
    tmp_var.data.data.i = 2;
    stable_add_var(sym_tab, "str_2", tmp_var);
    tmp_var.data.data.i = 3;
    stable_add_var(sym_tab, "str_3", tmp_var);
    tmp_var.data.data.i = 4;
    stable_add_var(sym_tab, "len_1", tmp_var);
    tmp_var.data.data.i = 5;
    stable_add_var(sym_tab, "len_2", tmp_var);
    tmp_var.data.data.i = 6;
    stable_add_var(sym_tab, "len_3", tmp_var);
    stable_print(sym_tab);

    //vytvorenie lokalnych premennych
    create_and_add_instruction(i_list, INST_PUSH_STRING, 0, 0, 0);
    create_and_add_instruction(i_list, INST_PUSH_STRING, 0, 0, 0);
    create_and_add_instruction(i_list, INST_PUSH_STRING, 0, 0, 0);
    create_and_add_instruction(i_list, INST_PUSH_INT, 0, 0, 0);
    create_and_add_instruction(i_list, INST_PUSH_INT, 0, 0, 0);
    create_and_add_instruction(i_list, INST_PUSH_INT, 0, 0, 0);

    ptr_to_table1 = stable_get_var(sym_tab, "str_1");
    ptr_to_table2 = stable_get_var(sym_tab, "str_2");
    ptr_to_table3 = stable_get_var(sym_tab, "len_1");

    create_and_add_instruction(i_list, INST_READ_STRING, &ptr_to_table1->data, 0, 0);
    create_and_add_instruction(i_list, INST_STR_LEN, &ptr_to_table3->data, &ptr_to_table1->data, 0);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table1->data, 0, 0);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table3->data, 0, 0);

    ptr_to_table3 = stable_get_var(sym_tab, "len_2");
    create_and_add_instruction(i_list, INST_READ_STRING, &ptr_to_table2->data, 0, 0);
    create_and_add_instruction(i_list, INST_STR_LEN, &ptr_to_table3->data, &ptr_to_table2->data, 0);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table2->data, 0, 0);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table3->data, 0, 0);

    ptr_to_table1 = stable_get_var(sym_tab, "str_1");
    ptr_to_table2 = stable_get_var(sym_tab, "str_2");
    ptr_to_table3 = stable_get_var(sym_tab, "str_3");
    create_and_add_instruction(i_list, INST_STR_CONCATENATE, &ptr_to_table3->data, &ptr_to_table1->data,
                               &ptr_to_table2->data);
    ptr_to_table2 = stable_get_var(sym_tab, "len_3");
    create_and_add_instruction(i_list, INST_STR_LEN, &ptr_to_table2->data, &ptr_to_table3->data, 0);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table3->data, 0, 0);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table2->data, 0, 0);

    create_and_add_instruction(i_list, INST_HALT, 0, 0, 0);

    interpret(i_list, sym_tab);

    dest_inst_list(&i_list);
    stable_destroy(&sym_tab);

*/
    printf("\n\nTEST 3\n"
                   "");

    i_list = init_inst_list();
    sym_tab = stable_init(10);


    tmp_var.data.arg_type = STACK_EBP;

    tmp_var.data.data.i = 1;
    stable_add_var(sym_tab, "INT.A", tmp_var);
    tmp_var.data.data.i = 2;
    stable_add_var(sym_tab, "str_1", tmp_var);
    tmp_var.data.data.i = 3;
    stable_add_var(sym_tab, "str_2", tmp_var);

    create_and_add_instruction(i_list, INST_PUSH_INT, 0, 0, 0);
    create_and_add_instruction(i_list, INST_PUSH_STRING, 0, 0, 0);
    create_and_add_instruction(i_list, INST_PUSH_STRING, 0, 0, 0);
    ptr_to_table1 = stable_get_var(sym_tab, "str_1");
    ptr_to_table2 = stable_get_var(sym_tab, "str_2");
    ptr_to_table3 = stable_get_var(sym_tab, "INT.A");
    create_and_add_instruction(i_list, INST_READ_STRING, &ptr_to_table1->data, 0, 0);
    create_and_add_instruction(i_list, INST_READ_STRING, &ptr_to_table2->data, 0, 0);

    create_and_add_instruction(i_list, INST_STR_CMP, &ptr_to_table3->data, &ptr_to_table2->data, &ptr_to_table1->data);
    create_and_add_instruction(i_list, INST_WRITE, &ptr_to_table3->data, 0, 0);

    create_and_add_instruction(i_list, INST_HALT, 0, 0, 0);

    interpret(i_list, sym_tab);


    dest_inst_list(&i_list);
    stable_destroy(&sym_tab);

    return 0;
}