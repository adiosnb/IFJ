#include <stdio.h>
#include "inter_table.h"
#include "instruction_list.h"
#include "instructions.h"
#include "interpret.h"
#include "inter_idea_recursion.h"

void debug_print_instruction_list(instruction_list_t* list){

    instruction_item_t *current = list->first;

    char *msg[] = {
            "INST_NOP",
            "INST_ADD",
            "INST_SUB",
            "INST_MUL",
            "INST_DIV",
            "INST_PUSH",
            "INST_POP",
            "INST_STORE",
            "INST_JMP",
            "INST_JEQ",
            "INST_JNEQ",
            "INST_JZ",
            "INST_CALL",
            "INST_RET",
            "INST_WRITE",
            "INST_READ_INT",
            "INST_READ_DOUBLE",
            "INST_READ_STRING",
            "INST_STR_LEN",
            "INST_STR_CMP",
            "INST_STR_FIND",
            "INST_STR_SORT",
            "INST_STR_SUBSTR",
            "INST_HALT",
            "INST_LABEL",
            NULL,
    };

    while (current != NULL){
        printf(        "┌──────────────────────────────────────┐\n"
                               "| Inst address: %5i %5i %5i      |\n"
                               "| Instruction : %15s        |\n"
                               "└──────────────────────────────────────┘\n",
                       current->instruction.addr1,
                       current->instruction.addr2,
                       current->instruction.addr3,
                       msg[current->instruction.type]);
        current = current->next;
    }
    printf("Instruction stream\n");

}

void debug_print_inter_table(inter_table_t *inter_tab){ // TODO naplnit tabulku inter aj stable

    int i = 0;
    inter_table_elem_t* current_table_element = inter_tab->arr[0]; // prvy element tabulky

    while (inter_tab->arr[i] == NULL || inter_tab->arr[i]->stab_next == NULL){
        i++;
        current_table_element = inter_tab->arr[i];
    }

    while (inter_tab->stab_size > i){
        printf(        "┌──────────────────────────────────────┐\n"
                               "| Key, arg_type %7i %7i        |\n"
                               "| Data : %15i        |\n"
                               "└──────────────────────────────────────┘\n",
                       current_table_element->stab_key,
                       current_table_element->stab_content.arg_type,
                       current_table_element->stab_content.data.i
                       );
        current_table_element = inter_tab->arr[i]->stab_next;

    }



}

int main() {

    argument_var_t tmp_var;
    instruction_list_t *i_list;
    inter_table_t *inter_tab;
    instruction_item_t *ptr_inst;

    i_list = init_inst_list();
    inter_tab = inter_table_init(1024);

//generovanie konstant do sym tabulky
    for (int i = 0; i < 20; i++) {
        tmp_var.arg_type = INTEGER;
        tmp_var.data.i = i;
        inter_table_add_var(inter_tab,i, &tmp_var);
    }

    for (int i = 20; i < 40; i++) {
        tmp_var.arg_type = INTEGER;
        tmp_var.data.i = (i - 20) * (-1);
        inter_table_add_var(inter_tab, i, &tmp_var);
    }


   /*for (int i = 0; i< 5000000; i++){
        create_and_add_instruction(i_list, INST_PUSH, 0, 0, 0);//push cisla 5
    }
*/

    //definicia argumentov
    tmp_var.arg_type = STACK_EBP;
    tmp_var.data.i = -3;
    inter_table_add_var(inter_tab, ARG_A, &tmp_var);
    tmp_var.arg_type = STACK_EBP;
    tmp_var.data.i = -2;
    inter_table_add_var(inter_tab, ARG_B, &tmp_var);

    //definicia lokalnej premennej
    tmp_var.arg_type = STACK_EBP;
    tmp_var.data.i = 1;
    inter_table_add_var(inter_tab, LOCAL_C, &tmp_var);

    //program skoci do main
    create_and_add_instruction(i_list, INST_JMP, MAIN, 0, 0);

    //funkcia ADD
    ptr_inst = create_and_add_instruction(i_list, INST_LABEL, 0, 0, 0);     //navestie instrukcie
    tmp_var.arg_type = INSTRUCTION;
    tmp_var.data.instruction = ptr_inst;
    inter_table_add_var(inter_tab, ADD, &tmp_var);
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
    inter_table_add_var(inter_tab, OVER_RECURSION, &tmp_var);
    create_and_add_instruction(i_list, INST_RET, 0, 0, 0);//koniec funkcie ADD



    //MAIN
    ptr_inst = create_and_add_instruction(i_list, INST_WRITE, 10, 0, 0); //echo number 10
    tmp_var.arg_type = INSTRUCTION;
    tmp_var.data.instruction = ptr_inst;
    inter_table_add_var(inter_tab, MAIN, &tmp_var); //ulozenie instrukcie do tabulky


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



    //debug_print_instruction_list(i_list);
    debug_print_inter_table(inter_tab);

    interpret(i_list, inter_tab);


    dest_inst_list(i_list);
    inter_table_destroy(&inter_tab);

    return 0;
}
