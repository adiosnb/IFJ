#ifndef _OP_PARSER_INCLUDED
#define _OP_PARSER_INCLUDED

#define MAX_TERMINALS   14
#define MAX_RULES       12

#include "../ial.h"
#include "../scanner.h"
#include "utils/dynamic_stack.h"

enum 
{
    UNDEF  = -1,
    BOTTOM = 13,
    C      = 100
};

#define BOOL    1000

const char op_table[][MAX_TERMINALS] =
{   //        0     1    2    3   4    5     6    7    8    9   10   11   12   13
    //        ==   !=    <    >   <=   >=    +    -    *    /   ID    (    )    $
    /* == */{'E', 'E', 'E', 'E', 'E', 'E', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* != */{'E', 'E', 'E', 'E', 'E', 'E', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* <  */{'E', 'E', 'E', 'E', 'E', 'E', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* >  */{'E', 'E', 'E', 'E', 'E', 'E', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* <= */{'E', 'E', 'E', 'E', 'E', 'E', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* >= */{'E', 'E', 'E', 'E', 'E', 'E', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* +  */{'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '>' },
    /* -  */{'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '>' },
    /* *  */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>' },
    /* /  */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>' },
    /* ID */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '_', '_', '>', '>' },
    /* (  */{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '_' },
    /* )  */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '_', '_', '>', '>' },
    /* $  */{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '_', '=' },
};

const int rule_table[][MAX_RULES] =
{
    {C, C, TOK_EQ, C, UNDEF},
    {C, C, TOK_NOTEQ, C, UNDEF},
    {C, C, TOK_LESS, C, UNDEF},
    {C, C, TOK_GREATER, C, UNDEF},
    {C, C, TOK_LE, C, UNDEF},
    {C, C, TOK_GE, C, UNDEF},
    {C, C, TOK_PLUS, C, UNDEF},
    {C, C, TOK_MINUS, C, UNDEF},
    {C, C, TOK_MUL, C, UNDEF},
    {C, C, TOK_DIV, C, UNDEF},
    {C, TOK_ID, UNDEF, UNDEF, UNDEF},
    {C, TOK_LEFT_PAR, C, TOK_RIGHT_PAR, UNDEF}
};

const int rule_len[MAX_RULES] =
{
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    2,
    4
};

int         find_right_side(const stack_t *const);
stack_t     get_reduce_symbols(const stack_t const*, stack_t *const);
const int*  get_rule(int);
void        stack_reduce_rule(stack_t *const, int);
expr_t get_top_terminal(stack_t *);

#endif
