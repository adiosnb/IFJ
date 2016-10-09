#include "stable.h"
#include "instruction_list.h"
#include "instructions.h"
#include "interpret.h"


int main(){

    argument_var_t tmp_var;
    instruction_list_t *i_list;
    stable_t *sym_tab;
    instruction_item_t *ptr_inst1, *ptr_inst;

    i_list = init_inst_list();
    //sym_tab =   TODO : urobit konstruktor pre sym tab

    for(int i = 0 ; i < 20; i++) {
        tmp_var.arg_type = IN_UNION;
        tmp_var.data.data_type = INTEGER;
        tmp_var.data.data.i = i;
        add_var(i, &tmp_var, sym_tab);
    }

    tmp_var.arg_type = ON_STACK; tmp_var.position_on_stack = -1;
    add_var(30, &tmp_var, sym_tab);
    tmp_var.arg_type = ON_STACK; tmp_var.position_on_stack = -2;
    add_var(31, &tmp_var, sym_tab);
    tmp_var.arg_type = ON_STACK; tmp_var.position_on_stack = 1;
    add_var(25, &tmp_var, sym_tab);


    create_and_add_instruction(i_list,INST_GOTO,50,0,0);
    create_and_add_instruction(i_list,INST_PUSH,6,0,0);
    create_and_add_instruction(i_list,INST_ADD,25,30,31);
    create_and_add_instruction(i_list,INST_WRITE,25,0,0);
    create_and_add_instruction(i_list,INST_RET,0,0,0);

    ptr_inst = create_and_add_instruction(i_list,INST_PUSH,5,0,0);
    tmp_var.arg_type = INSTRUCTION; tmp_var.instruction = ptr_inst;
    add_var(50,&tmp_var,sym_tab);
    create_and_add_instruction(i_list,INST_PUSH,6,0,0);
    create_and_add_instruction(i_list,INST_CALL,5,0,0);


    interpret(i_list,sym_tab);


    dest_inst_list(i_list);

    return 0;
}
