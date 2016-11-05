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
        inter_table_add_var(sym_tab,i, &tmp_var);
    }

    for (int i = 20; i < 40; i++) {
        tmp_var.arg_type = INTEGER;
        tmp_var.data.i = (i - 20) * (-1);
        inter_table_add_var(sym_tab, i, &tmp_var);
    }


    for (int i = 0; i< 5000000; i++){
        create_and_add_instruction(i_list, INST_PUSH, 0, 0, 0);//push cisla 5
    }


    //definicia argumentov
    tmp_var.arg_type = STACK_EBP;
    tmp_var.data.i = -3;
    inter_table_add_var(sym_tab, ARG_A, &tmp_var);
    tmp_var.arg_type = STACK_EBP;
    tmp_var.data.i = -2;
    inter_table_add_var(sym_tab, ARG_B, &tmp_var);

    //definicia lokalnej premennej
    tmp_var.arg_type = STACK_EBP;
    tmp_var.data.i = 1;
    inter_table_add_var(sym_tab, LOCAL_C, &tmp_var);

    //program skoci do main
    create_and_add_instruction(i_list, INST_JMP, MAIN, 0, 0);

    //funkcia ADD
    ptr_inst = create_and_add_instruction(i_list, INST_LABEL, 0, 0, 0);     //navestie instrukcie
    tmp_var.arg_type = INSTRUCTION;
    tmp_var.data.instruction = ptr_inst;
    inter_table_add_var(sym_tab, ADD, &tmp_var);
    create_and_add_instruction(i_list, INST_PUSH, 0, 0, 0);//lokalna premenna
    create_and_add_instruction(i_list, INST_ADD, LOCAL_C, ARG_A, ARG_B);
    create_and_add_instruction(i_list, INST_WRITE, LOCAL_C, 0, 0);
    create_and_add_instruction(i_list, INST_JZ, OVER_RECURSION, LOCAL_C, 0);

    //rekurzivne volanie funkcie
    create_and_add_instruction(i_list, INST_PUSH, LOCAL_C, 0, 0);//push cisla 5
    create_and_add_instruction(i_list, INST_PUSH, 21, 0, 0);//push cisla 1
    create_and_add_instruction(i_list, INST_CALL, ADD, 0, 0);//volanie funkcie ADD
    create_and_add_instruction(i_list, INST_POP, 0, 0, 0);
    create_and_add_instruction(i_list, INST_POP, 0, 0, 0);


    //preskocenie na koniec programu
    ptr_inst = create_and_add_instruction(i_list, INST_LABEL, 0, 0, 0);
    tmp_var.arg_type = INSTRUCTION;
    tmp_var.data.instruction = ptr_inst;
    inter_table_add_var(sym_tab, OVER_RECURSION, &tmp_var);
    create_and_add_instruction(i_list, INST_RET, 0, 0, 0);//koniec funkcie ADD



    //MAIN
    ptr_inst = create_and_add_instruction(i_list, INST_WRITE, 10, 0, 0); //echo number 10
    tmp_var.arg_type = INSTRUCTION;
    tmp_var.data.instruction = ptr_inst;
    inter_table_add_var(sym_tab, MAIN, &tmp_var); //ulozenie instrukcie do tabulky


    create_and_add_instruction(i_list, INST_PUSH, 0, 0, 0);//push cisla 5
    create_and_add_instruction(i_list, INST_PUSH, 0, 0, 0);//push cisla -1
    create_and_add_instruction(i_list, INST_CALL, ADD, 0, 0);//volanie funkcie ADD
    create_and_add_instruction(i_list, INST_POP, 0, 0, 0);
    create_and_add_instruction(i_list, INST_POP, 0, 0, 0);


    create_and_add_instruction(i_list, INST_PUSH, 5, 0, 0);//push cisla 5
    create_and_add_instruction(i_list, INST_PUSH, 21, 0, 0);//push cisla -1
    create_and_add_instruction(i_list, INST_CALL, ADD, 0, 0);//volanie funkcie ADD
    create_and_add_instruction(i_list, INST_POP, 0, 0, 0);
    create_and_add_instruction(i_list, INST_POP, 0, 0, 0);

    create_and_add_instruction(i_list, INST_HALT, 0, 0, 0); //koniec porgramu





    interpret(i_list, sym_tab);


    dest_inst_list(i_list);
    inter_table_destroy(&sym_tab);

    return 0;
}
