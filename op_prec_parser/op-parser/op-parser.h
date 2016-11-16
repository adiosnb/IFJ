#ifndef _OP_PARSER_INCLUDED
#define _OP_PARSER_INCLUDED

#define MAX_TERMINALS   14
#define MAX_RULES       12
#define BOTTOM          13

#define UNDEF   -1
#define C       100

extern const char op_table[][MAX_TERMINALS];

extern  int rule_table[][MAX_RULES]; 
extern const int rule_len[]; 


int find_right_side(const stack_t *const);
stack_t get_reduce_symbols(const stack_t const*, stack_t *const);
int *get_rule(int);
void stack_reduce_rule(stack_t *const, int);

#endif
