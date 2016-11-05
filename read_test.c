#include "inter_table.h"
#include "instruction_list.h"
#include "instructions.h"
#include "interpret.h"
#include "inter_idea_recursion.h"


int main() {

    argument_var_t tmp_var;
    instruction_list_t *i_list;
    inter_table_t *sym_tab;
    instruction_item_t *ptr_inst;

    i_list = init_inst_list();
    sym_tab = inter_table_init(1024);

//generovanie konstant do sym tabulky
    for (int i = 0; i < 20; i++) {
        tmp_var.arg_type = INTEGER;
        tmp_var.data.i = i;
        inter_table_add_var(i, &tmp_var, sym_tab);
    }

    for (int i = 20; i < 40; i++) {
        tmp_var.arg_type = DOUBLE;
        tmp_var.data.i = (i - 20) * (-1);
        inter_table_add_var(i, &tmp_var, sym_tab);
    }


    //definicia lokalnej premennej
    tmp_var.arg_type = STACK_EBP;
    tmp_var.data.i = 1;
    inter_table_add_var(LOCAL_C, &tmp_var, sym_tab);

    tmp_var.arg_type = STACK_EBP;
    tmp_var.data.i = 2;
    inter_table_add_var(LOCAL_D, &tmp_var, sym_tab);

    //program skoci do main
    create_and_add_instruction(i_list, INST_JMP, MAIN, 0, 0);



    //MAIN
    ptr_inst = create_and_add_instruction(i_list, INST_WRITE, 10, 0, 0); //echo number 10
    tmp_var.arg_type = INSTRUCTION;
    tmp_var.data.instruction = ptr_inst;
    inter_table_add_var(MAIN, &tmp_var, sym_tab); //ulozenie instrukcie do tabulky

    create_and_add_instruction(i_list, INST_PUSH, 0, 0, 0);//push cisla 5
    create_and_add_instruction(i_list, INST_PUSH, 25, 0, 0);//push cisla 5

    create_and_add_instruction(i_list,INST_READ_INT,LOCAL_C,0,0);
    create_and_add_instruction(i_list, INST_WRITE, LOCAL_C, 0, 0);

    create_and_add_instruction(i_list,INST_READ_DOUBLE,LOCAL_D,0,0);
    create_and_add_instruction(i_list, INST_WRITE, LOCAL_D, 0, 0);


    create_and_add_instruction(i_list, INST_HALT, 0, 0, 0); //koniec porgramu





    interpret(i_list, sym_tab);


    dest_inst_list(i_list);
    inter_table_destroy(&sym_tab);

    return 0;
}
