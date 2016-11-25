#include "op-parser.h"
#include "../interpret.h" 
stab_t* staticSym = NULL;
char* parser_class = "test";
char* parser_fun = "fun";

instruction_list_t* insProgram = NULL;
int main(void)
{
    staticSym = stable_init(1024);

    data_t dt;
    dt.type = INTEGER;
    dt.data.arg_type = INTEGER;
    dt.data.data.i = 666;
    stable_add_var(staticSym, "test.a", dt);
    stable_add_var(staticSym, "test.b", dt);

    dt.data.arg_type = ON_TOP;
    data_t* top = stable_add_var(staticSym, "ifj16.top", dt);

    insProgram = init_inst_list();

    stable_print(staticSym);
    fprintf(stderr, "\n==========\nResult type: %i\n", parse_expression(true, true));

create_and_add_instruction(insProgram, INST_WRITE, &top->data,0,0);
create_and_add_instruction(insProgram, INST_HALT, 0,0,0);
// -------------------------------
    inst_list_print(insProgram);

	interpret(insProgram,staticSym);
	return 0;
}
