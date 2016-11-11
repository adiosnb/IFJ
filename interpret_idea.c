#include "inter_table.h"
#include "interpret.h"


int main(){

    argument_var_t tmp_var, *ptr_to_tab;
    instruction_list_t *i_list;
    inter_table_t *sym_tab;
    instruction_item_t *ptr_inst;

    i_list = init_inst_list();
    sym_tab = inter_table_init(1024);

    for(int i = 0 ; i < 20; i++) {
        tmp_var.arg_type = INTEGER;
        tmp_var.data.i = i;
        inter_table_add_var(sym_tab, (unsigned)i, &tmp_var);
    }

    tmp_var.arg_type = STACK_EBP; tmp_var.data.i = -3;
    inter_table_add_var(sym_tab, 30, &tmp_var);
    tmp_var.arg_type = STACK_EBP; tmp_var.data.i = -2;
    inter_table_add_var(sym_tab, 31, &tmp_var);
    tmp_var.arg_type = STACK_EBP; tmp_var.data.i = 1;
    inter_table_add_var(sym_tab, 25, &tmp_var);
    tmp_var.arg_type = ON_STACK; tmp_var.data.i = 1;
    inter_table_add_var(sym_tab, 24, &tmp_var);

    inter_table_add_var(sym_tab, 50, &tmp_var);
    inter_table_add_var(sym_tab, 100, &tmp_var);

    //sorry viem ze to vyzera ako keby to kokot pisal, ale ide iba o na rychlo zbuchany prototyp

    create_and_add_instruction(i_list, INST_JMP, inter_table_get_var(sym_tab,50) , 0, 0);                //preskocenie definicie funkcie


    ptr_inst = create_and_add_instruction(i_list, INST_LABEL, 0, 0, 0);     //navestie funkcie
    ptr_to_tab = inter_table_get_var(sym_tab, 100);
    ptr_to_tab->arg_type = INSTRUCTION;
    ptr_to_tab->data.instruction = ptr_inst;


    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab,30) ,0,0);
    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab,31) ,0,0);

    create_and_add_instruction(i_list, INST_PUSH, inter_table_get_var(sym_tab,0), 0, 0);                 //lokalna premenna
    create_and_add_instruction(i_list,INST_ADD,inter_table_get_var(sym_tab,25) ,inter_table_get_var(sym_tab,30) ,inter_table_get_var(sym_tab,31));
    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab,25) ,0,0);
    create_and_add_instruction(i_list, INST_RET, inter_table_get_var(sym_tab,2), 0, 0);                  //koniec funkcie


    ptr_inst = create_and_add_instruction(i_list, INST_LABEL, 0, 0, 0);      //predavanie premennych hodnotov
    ptr_to_tab = inter_table_get_var(sym_tab, 50);
    ptr_to_tab->arg_type = INSTRUCTION;
    ptr_to_tab->data.instruction = ptr_inst;

    create_and_add_instruction(i_list, INST_PUSH, inter_table_get_var(sym_tab,5), 0, 0);
    create_and_add_instruction(i_list, INST_PUSH, inter_table_get_var(sym_tab,6), 0, 0);                 //predavanie premennych hodnotov
    create_and_add_instruction(i_list, INST_CALL, inter_table_get_var(sym_tab,100), 0, 0);                 //volanie funkcie
    create_and_add_instruction(i_list,INST_POP,0,0,0);
    create_and_add_instruction(i_list,INST_POP,0,0,0);

    create_and_add_instruction(i_list, INST_PUSH, inter_table_get_var(sym_tab,9), 0, 0);
    create_and_add_instruction(i_list, INST_PUSH, inter_table_get_var(sym_tab,9), 0, 0);                 //predavanie premennych hodnotov
    create_and_add_instruction(i_list, INST_CALL, inter_table_get_var(sym_tab,100), inter_table_get_var(sym_tab,19), 0);                 //volanie funkcie
    create_and_add_instruction(i_list, INST_POP, 0, 0, 0);
    create_and_add_instruction(i_list, INST_POP, 0, 0, 0);
    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab,19),0,0);


    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab,2),0,0);                    //vypisanie premennej v tabulke symbolov
    create_and_add_instruction(i_list,INST_PUSH,inter_table_get_var(sym_tab,15),0,0);
    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab,24),0,0);
    create_and_add_instruction(i_list,INST_POP,0,0,0);
    create_and_add_instruction(i_list,INST_PUSH,inter_table_get_var(sym_tab,9),0,0);
    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab,24),0,0);

    create_and_add_instruction(i_list,INST_SUB,inter_table_get_var(sym_tab,10),inter_table_get_var(sym_tab,6),inter_table_get_var(sym_tab,3));
    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab,10),0,0);

    create_and_add_instruction(i_list,INST_MUL,inter_table_get_var(sym_tab,10),inter_table_get_var(sym_tab,6),inter_table_get_var(sym_tab,3));
    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab,10),0,0);

    create_and_add_instruction(i_list,INST_DIV,inter_table_get_var(sym_tab,10),inter_table_get_var(sym_tab,6),inter_table_get_var(sym_tab,4));
    create_and_add_instruction(i_list,INST_WRITE,inter_table_get_var(sym_tab, 10),0,0);

    create_and_add_instruction(i_list, INST_HALT, 0, 0, 0); //koniec porgramu


    interpret(i_list,sym_tab);


    dest_inst_list(i_list);
    inter_table_destroy(&sym_tab);

    return 0;
}
