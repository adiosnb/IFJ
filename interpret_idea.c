#include "stable.h"
#include "instruction_list.h"
#include "instructions.h"
#include "interpret.h"


int main(){

    argument_var_t tmp_var;
    instruction_list_t *i_list;
    stab_t *sym_tab;
    instruction_item_t *ptr_inst;

    i_list = init_inst_list();
    sym_tab = stable_init(1024);

    for(int i = 0 ; i < 20; i++) {
        tmp_var.arg_type = INTEGER;
        tmp_var.data.i = i;
        stable_add_var(i, &tmp_var, sym_tab);
    }

    tmp_var.arg_type = STACK_EBP; tmp_var.data.i = -3;
    stable_add_var(30, &tmp_var, sym_tab);
    tmp_var.arg_type = STACK_EBP; tmp_var.data.i = -2;
    stable_add_var(31, &tmp_var, sym_tab);
    tmp_var.arg_type = STACK_EBP; tmp_var.data.i = 1;
    stable_add_var(25, &tmp_var, sym_tab);
    tmp_var.arg_type = ON_STACK; tmp_var.data.i = 1;
    stable_add_var(24, &tmp_var, sym_tab);


    //sorry viem ze to vyzera ako keby to kokot pisal, ale ide iba o na rychlo zbuchany prototyp

    create_and_add_instruction(i_list, INST_GOTO, 50, 0, 0);                //preskocenie definicie funkcie
    ptr_inst = create_and_add_instruction(i_list, INST_LABEL, 0, 0, 0);     //navestie instrukcie
    tmp_var.arg_type = INSTRUCTION;
    tmp_var.data.instruction = ptr_inst;
    stable_add_var(100, &tmp_var, sym_tab);

    create_and_add_instruction(i_list,INST_WRITE,30,0,0);
    create_and_add_instruction(i_list,INST_WRITE,31,0,0);

    create_and_add_instruction(i_list, INST_PUSH, 0, 0, 0);                 //lokalna premenna
    create_and_add_instruction(i_list,INST_ADD,25,30,31);
    create_and_add_instruction(i_list,INST_WRITE,25,0,0);
    create_and_add_instruction(i_list, INST_RET, 0, 0, 0);                  //koniec funkcie


    ptr_inst = create_and_add_instruction(i_list, INST_LABEL, 0, 0, 0);      //predavanie premennych hodnotov
    tmp_var.arg_type = INSTRUCTION; tmp_var.data.instruction = ptr_inst;
    stable_add_var(50, &tmp_var, sym_tab);
    create_and_add_instruction(i_list, INST_PUSH, 5, 0, 0);
    create_and_add_instruction(i_list, INST_PUSH, 6, 0, 0);                 //predavanie premennych hodnotov
    create_and_add_instruction(i_list, INST_CALL, 100, 0, 0);                 //volanie funkcie
    create_and_add_instruction(i_list,INST_POP,0,0,0);
    create_and_add_instruction(i_list,INST_POP,0,0,0);



    create_and_add_instruction(i_list,INST_WRITE,2,0,0);                    //vypisanie premennej v tabulke symbolov
    create_and_add_instruction(i_list,INST_PUSH,15,0,0);
    create_and_add_instruction(i_list,INST_WRITE,24,0,0);
    create_and_add_instruction(i_list,INST_POP,0,0,0);
    create_and_add_instruction(i_list,INST_PUSH,9,0,0);
    create_and_add_instruction(i_list,INST_WRITE,24,0,0);


    create_and_add_instruction(i_list, INST_HALT, 0, 0, 0); //koniec porgramu


    interpret(i_list,sym_tab);


    dest_inst_list(i_list);

    return 0;
}
