#include <stdio.h>

#include "../scanner/scanner_alt.h"
#include "../utils/dynamic_stack.h"
#include "../error/error.h"
#include "op-parser.h"

//#define NDEBUG

extern const char op_table[][MAX_TERMINALS];
extern const int rule_table[][MAX_RULES];
extern const int rule_len[MAX_RULES];

static inline t_token validate_ins(t_token s)
{
    // just for now to see where is end of input tokens string
    if (s.type == TOK_EOF)
    {
        s.data.op = '$';
        s.type = BOTTOM;
    }

    switch (s.type)
    {
        case TOK_SPECIAL_ID:
        case TOK_LITERAL:
        case TOK_CONST:
        case TOK_DOUBLECONST: if (s.data.op != '$') s.type = TOK_ID;
    }

    
    if (!(((int)s.type >= TOK_EQ && s.type <= BOTTOM) || s.type == C))
    // TODO: validate input symbol and print its string....how? scanner is not capable of doing that
        error_and_die(ERR_SYNTAX, "Expression: invalid input symbol %i", s.type);
    return s;
}

// top is top terminal of pushdown automaton and ins is input symbol
static void generate_syntax_error(t_token topt, t_token inst)
{
    topt = validate_ins(topt);
    inst = validate_ins(inst);
    
    int top = topt.type;
    int ins = inst.type; 

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
}

#define IS_OPERATOR(op)    (op == TOK_NOTEQ || op == TOK_LESS || op == TOK_GREATER || op == TOK_LE || op == TOK_GE || op == TOK_PLUS || op == TOK_MINUS || op == TOK_MUL || op == TOK_DIV)    


static void generate_reduction_error(t_token topt, t_token inst)
{
    topt = validate_ins(topt);
    inst = validate_ins(inst);
    
    int top = topt.type;
    int ins = inst.type; 

    if (top == TOK_LEFT_PAR && ins == TOK_RIGHT_PAR)
        error_and_die(ERR_SYNTAX, "Expression: no expression between parentheses"); 
    if (IS_OPERATOR(top) && (ins != TOK_ID || ins != TOK_LEFT_PAR))
        error_and_die(ERR_SYNTAX, "Expression: missing right operand");
    if (top == C && !IS_OPERATOR(ins))
        error_and_die(ERR_SYNTAX, "Expression: missing left operand");
}

static char compare_token(int pda_symbol, int input_symbol)
{
    
    return op_table[pda_symbol][input_symbol];
}

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
    printf("pd = '%s' and ins = '%s'\n", tokens[b], tokens[a]);
}
// TODO: symbol table checking data types

static inline t_token get_next_token(void)
{
    getToken();
    return validate_ins(g_lastToken);
}

void parse_expression(void)
{
    if (!scanner_openFile("input_test.txt"))
    {
        fprintf(stderr, "Cannot open file!\n");
        return;
    }

    stack_t     handle = stack_ctor(); // dynamic allocation
    stack_t     pda = stack_ctor();    // dynamic allocation

    t_token     ins = get_next_token();
    t_token     top_terminal;
    t_token     top_terminal_tmp;

    // HACK HACK: BOTTOM equals to TOK_SPECIAL_ID, adding dollar to find out BOTTOM
    t_token     bottom = {.type = BOTTOM, .data.op = '$'};

    stack_push(&pda, bottom);
    do
    {
        top_terminal = get_top_terminal(&pda);
        
        int result = compare_token(top_terminal.type, ins.type);     

        switch(result)
        {
                case '=':
                        handle = stack_clear(&handle);
                        handle = get_reduce_symbols(&pda, &handle);

                        if (!stack_empty(&handle))
                        {
                               if (find_right_side(&handle) == -1)
                                   generate_reduction_error(*stack_top(&pda), ins);
                        }
                        // push input symbol 
                        stack_push(&pda, g_lastToken);

                        // read next token
                        ins = get_next_token();
                        break;
                case '<':
                        // replacement of top terminal with top terminal and '<' (beginning of handle)
                        stack_add_handle_symbol(&pda, top_terminal.type);

                        // push input symbol
                        stack_push(&pda, g_lastToken);

                        // read next token
                        ins = get_next_token();
                        break;
                case '>':
                        handle = stack_clear(&handle);
                        handle = get_reduce_symbols(&pda, &handle);

                        if (!stack_empty(&handle))
                        {
                            int res = find_right_side(&handle);

                            if (res != -1)
                            {
                                const int *left_side = get_rule(res);

                                // switch topmost pda rule string with left side
                                stack_reduce_rule(&pda, *left_side); 
                                // postfix actions
                                if (res != 11)
                                    printf("%s ", tokens[res]);
                            }
                            else
                               generate_reduction_error(*stack_top(&pda), ins);
                        }
                        else    
                            error_and_die(ERR_SYNTAX, "Handle does not exist!\n");

                        break; 
                case '_': 
                        if (top_terminal.type == TOK_LEFT_PAR && ins.type == BOTTOM) 
                            error_and_die(ERR_SYNTAX, "Missing right parenthesis!");
                        if (top_terminal.type == BOTTOM && ins.type == TOK_RIGHT_PAR) 
                            error_and_die(ERR_SYNTAX, "Unbalanced '('");
                        generate_syntax_error(*stack_top(&pda), ins); 
                        return;
            }
            top_terminal_tmp = get_top_terminal(&pda);
    }
    while (ins.type != BOTTOM || (int)top_terminal_tmp.type != BOTTOM);

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
    stack_t string = *handle; 

    if (stack == NULL )
        return string;

    for (long end = stack->top; !is_stack_bottom(stack); end--)
    {
        if (stack->elem[end].type != '<')
        {
            stack_push(&string, validate_ins(stack->elem[end]));
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

    const int   *rule;
    t_token     *top_terminal;
    int          left_side_len;
    bool         is_rule;

    for (int i = 0; i != MAX_RULES; i++)
    {
        // rule len without left side
        left_side_len = rule_len[i]-1;

        if (stack_elem_count(handle) != left_side_len) 
            continue;

        // skip first symbol, it is left hand side!
        rule = &rule_table[i][1];
        top_terminal = &handle->elem[0];
        is_rule = true;

        // walking through the rule and checking if rule symbols equal to handle
        for (int j = 0; j != left_side_len; j++, rule++, top_terminal++)
        {
            if (*rule != (int)top_terminal->type)
            {
                is_rule = false;
                break;
            }
        }

        if (is_rule)
            return i;

    }

    return -1;
}

const int *get_rule(int rule_idx)
{
    if (rule_idx < 0 || rule_idx >= MAX_RULES)
        return NULL;

    return &rule_table[rule_idx][0];
}

t_token get_top_terminal(stack_t *stack) 
{ 

    long top = stack->top;
    obj_t cur_token;

    for (; top != -1; top--)
    {
        cur_token = validate_ins(stack->elem[top]);

        if ((int)cur_token.type >= TOK_EQ && cur_token.type <= BOTTOM)
            return cur_token;
    }
    return cur_token;
}


int main(void)
{
	parse_expression();
	return 0;
}
