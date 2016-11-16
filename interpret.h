#ifndef INTERPRET
#define INTERPRET

#include <stdlib.h>
#include <stdio.h>

#include "stable.h"
#include "instruction_list.h"
#include "stack.h"

extern stack_t *glob_stack;
extern stab_t *glob_stable;
extern instruction_list_t *glob_ins_list;

int interpret(instruction_list_t *instruction_list, stab_t *stable);

void halt();

void add();      //DEST OP1 OP2
void sub();      //dest op1 op2
void mul();      //dest op1 op2
void divisoin();      //dest op1 op2
void expr_add();
void expr_sub();
void expr_mul();
void expr_div();
void write();    //src
void read_int();//dst
void read_double();//dst
void read_string();//dst
void call();    //instruction_item_ptr
void call_print();
void call_str_cmp();
void call_str_find();
void call_str_sort();
void call_str_substr();
void ret();     //
void push();     //src
void pop();     //
void store();   //dst   src
void inst_jump();//instruction_item_ptr
void jump_zero();//instruction_item_ptr  num_fot_condition
void jump_equal();//instruction_item_ptr  num_for_condition num_for_condition
void jump_not_equal();//instruction_item_ptr  num_for_condition num_for_condition
void interpret_str_init();
void interpret_str_reinit();
void str_len();//dst  src
void concatenate();

void interpret_str_cmp();
void interpret_str_find();
void interpret_str_sort();
void interpret_str_substr();


#endif //INTERPRET