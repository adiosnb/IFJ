#include <stdio.h>

#include "../scanner/scanner_alt.h"
#include "../utils/dynamic_stack.h"
#include "../error/error.h"
#include "op-parser.h"

//#define NDEBUG

const char op_table[][MAX_TERMINALS] =
{
    //        ==   !=    <    >   <=   >=    +    -    *    /   ID    (    )    $
    /* == */{'>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* != */{'>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* <  */{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* >  */{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* <= */{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* >= */{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>' },
    /* +  */{'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '>' },
    /* -  */{'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '>' },
    /* *  */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>' },
    /* /  */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>' },
    /* ID */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '_', '_', '>', '>' },
    /* (  */{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '_' },
    /* )  */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '_', '_', '>', '>' },
    /* $  */{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '_', '=' },
};

int rule_table[][MAX_RULES] =
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

const int rule_len[] =
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

static inline int validate_ins(int a)
{
   
    switch (a)
    {
        case TOK_SPECIAL_ID:
        case TOK_LITERAL:
        case TOK_CONST:
        case TOK_DOUBLECONST: a = TOK_ID;
    }

    if (a == TOK_EOF)
        a = BOTTOM;

    // TODO: validate input symbol and print its string....how? scanner is not capable of doing that
    if (a >= TOK_EQ && a <= BOTTOM)
        return a;
    else
        error_and_die(ERR_SYNTAX, "Expression: invalid input symbol %s", g_lastToken.data.string);
    return -1;    
}

// top is top terminal of pushdown automaton and ins is input symbol
void generate_syntax_error(int top, int ins)
{
    top = validate_ins(top);
    ins = validate_ins(ins);
    
    if (top == TOK_ID && ins == TOK_ID)
        error_and_die(ERR_SYNTAX, "Expression: missing operator between two identifiers");
    if (top == TOK_ID && ins == TOK_LEFT_PAR)
        error_and_die(ERR_SYNTAX, "Expression: missing operator between identifier and '('");
    if (top == TOK_LEFT_PAR && ins == BOTTOM)
        error_and_die(ERR_SYNTAX, "Expression: missing ')'"); 
    if (top == TOK_RIGHT_PAR && ins == TOK_ID)
        error_and_die(ERR_SYNTAX, "Expression: missing operator between ')' and identifier"); 
    if (top == TOK_RIGHT_PAR && ins == TOK_LEFT_PAR)
        error_and_die(ERR_SYNTAX, "Expression: missing operator between ')' and '('"); 
    if (top == BOTTOM && ins == TOK_RIGHT_PAR)
        error_and_die(ERR_SYNTAX, "Expression: unbalanced ')'");
}

#define IS_OPERATOR(op)    (op == TOK_NOTEQ || op == TOK_LESS || op == TOK_GREATER || op == TOK_LE || op == TOK_GE || op == TOK_PLUS || op == TOK_MINUS || op == TOK_MUL || op == TOK_DIV)    


static void generate_reduction_error(int top, int ins)
{
    top = validate_ins(top);
    ins = validate_ins(ins);
    
    if (IS_OPERATOR(top) && (ins != TOK_ID || ins != TOK_LEFT_PAR))
        error_and_die(ERR_SYNTAX, "Expression: missing operator between two identifiers");
    if (top == TOK_RIGHT_PAR && ins == TOK_LEFT_PAR)
        error_and_die(ERR_SYNTAX, "Expression: no epxression between parentheses"); 
}



static char compare_token(int pda_symbol, int input_symbol)
{
    
    return op_table[pda_symbol][input_symbol];
}

int a,b;
int result;
int temp = 40;

const char *tokens[] = 
{
    "==", "!=", "<", ">", "<=", ">=", "+", "-", "*", "/", "i", "(", ")", "$", [100] = "E", [60] = "<"
};

void print_stack(stack_t *stack)
{
    for (int i = 0; i <= stack->top; i++)
        if (stack->elem[i].type != BOTTOM)
            printf("%s ",  tokens[stack->elem[i].type]);
    printf("\n");
}

void print_input(int a, int b)
{
    printf("pd = '%s'\nins = '%s'\n", tokens[b], tokens[a]);
}
// TODO: symbol table checking data types

void parse_expression(void)
{
    if (!scanner_openFile("input_test.txt"))
    {
        fprintf(stderr, "Cannot open file!\n");
        return;
    }
    stack_t handle = stack_ctor();
    stack_t pda = stack_ctor();
    int c =  validate_ins(getToken());
    t_token *top_terminal;
    t_token *top_terminal_tmp;

    t_token bottom = {.type = BOTTOM};
    stack_push(&pda, bottom);
    do
    {
        top_terminal = get_top_terminal(&pda);
        
        int result = compare_token(top_terminal->type, c);     

        switch(result)
        {
                case '=':
                        // push input symbol 
                        stack_push(&pda, g_lastToken);
                        // read next token
                        c =  validate_ins(getToken());
                        break;
                case '<':
                        // replacement of top terminal with top terminal and '<' (beginning of handle)
                        stack_add_handle_symbol(&pda, top_terminal->type);

                        // push input symbol
                        stack_push(&pda, g_lastToken);

                        // read next token
                        c =  validate_ins(getToken());
                        break;
                case '>':
                        handle = get_reduce_symbols(&pda, &handle);

                        if (!stack_empty(&handle))
                        {
                            int res = find_right_side(&handle);

                            if (res != -1)
                            {
                                int *left_side = get_rule(res);

                                // switch topmost pda rule string with left side
                                stack_reduce_rule(&pda, *left_side); 
                                // postfix actions
                                if (res != 11)
                                    printf("%s ", tokens[res]);
                            }
                            else
                            {
                                error_and_die(ERR_SYNTAX, "Cannot find right side rule!\n");
                            }
                        }
                        else    
                            error_and_die(ERR_SYNTAX, "Handle does not exist!\n");

                        break; 
                case '_':
                        generate_syntax_error(top_terminal->type, c); 
                        return;
            }
            top_terminal_tmp = get_top_terminal(&pda);
    }
    while (c != BOTTOM || (int)top_terminal_tmp->type != BOTTOM);

    putchar('\n');
    handle = stack_dtor(&handle);
    pda = stack_dtor(&pda);
}

static inline bool is_stack_bottom(const stack_t *const stack)
{
    if (stack != NULL)
        return stack->top == 0 && stack->elem[0].type == BOTTOM;
    return true;
}

stack_t get_reduce_symbols(const stack_t *const stack, stack_t *const handle)
{
    *handle = stack_clear(handle);
    stack_t string = *handle; 

    if (stack == NULL )
        return string;

    long end = stack->top;

    for (; !is_stack_bottom(stack); end--)
    {
        if (stack->elem[end].type != '<')
        {
            stack_push(&string, stack->elem[end]);
        }
        else
        {
            long str_end = string.top;
            long str_start = 0;

            for (; str_start < str_end; str_end--, str_start++)
            {
                t_token tmp = string.elem[str_start];
                string.elem[str_start] = string.elem[str_end];
                string.elem[str_end] = tmp;
            }

            return string;
        }
    }
    return string;
}

void stack_reduce_rule(stack_t *const stack, int left_side)
{
    if (stack == NULL)
        return;

    long end; 

    for (end = stack->top; !is_stack_bottom(stack); end--)
    {
       if (stack->elem[end].type == '<')
          break; 
    }

    if (stack->elem[end].type == '<')
    {
       t_token left = {.type = left_side}; 
       stack->top = end-1;
       stack_push(stack, left); 
    }
}

int find_right_side(const stack_t *const handle)
{
    if (handle == NULL)
        return -1;

    int     *rule;
    t_token *top_terminal;
    int     left_side_len;
    bool    is_rule;

    for (int i = 0; i != MAX_RULES; i++)
    {
        left_side_len = rule_len[i]-1;

        if (stack_elem_count(handle) != left_side_len) 
            continue;

        // skip first symbol, it is left hand side!
        rule = &rule_table[i][1];
        top_terminal = &handle->elem[0];
        is_rule = true;

        //for (; (*rule != UNDEF) && ((int)top_terminal->type != BOTTOM) && ((int)top_terminal->type == *rule); top_terminal--, rule++)  
        for (int j = 0; j != left_side_len; j++, rule++, top_terminal++)
            if (*rule != (int)top_terminal->type)
            {
                is_rule = false;
                break;
            }

        if (is_rule)
            return i;
    }

    return -1;
}

int *get_rule(int rule_idx)
{
    if (rule_idx < 0 || rule_idx >= MAX_RULES)
        return NULL;

    return &rule_table[rule_idx][0];
}

int main(void)
{
	parse_expression();
	return 0;
}
