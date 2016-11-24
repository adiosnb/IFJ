#include <stdio.h>

#include "../scanner.h"
#include "utils/dynamic_stack.h"
#include "../error.h"
#include "../instruction_list.h"
#include "op-parser.h"
#include "../ial.h"

//#define NDEBUG

extern const char op_table[][MAX_TERMINALS];
extern const int rule_table[][MAX_RULES];
extern const int rule_len[MAX_RULES];
extern stab_t* staticSym;
extern instruction_list_t* insProgram;

void parser_clean(void) { }

bool is_it_assign = false;

static inline expr_t validate_ins(t_token s)
{
    expr_t exp;
    exp.type = s.type;
    // just for now to see where is end of input tokens string
   
    if (is_it_assign && exp.type == TOK_DELIM)
    {
        exp.type = BOTTOM;
        ungetToken();
        return exp;
    }

    int op = 0;
    if (exp.type == TOK_EOF)
    {
        op = '$';
        exp.type = BOTTOM;
    }

    switch (exp.type)
    {
        case TOK_SPECIAL_ID:
        case TOK_LITERAL:
        case TOK_CONST:
        case TOK_DOUBLECONST: 
		if (op != '$') 
			exp.type = TOK_ID;
    }

    if (!(((int)exp.type >= TOK_EQ && exp.type <= BOTTOM) || exp.type == C))
    // TODO: validate input symbol and print its string....how? scanner is not capable of doing that
        error_and_die(SYNTAX_ERROR, "Expression: invalid input symbol %i", exp.type);

    return exp;
}


// top is top terminal of pushdown automaton and ins is input symbol
static void generate_syntax_error(expr_t topt, expr_t inst)
{
    //topt = validate_ins(topt);
    //inst = validate_ins(inst);
    
    int top = topt.type;
    int ins = inst.type; 

    if (top == TOK_ID && ins == TOK_ID)
        error_and_die(SYNTAX_ERROR, "Expression: missing operator between two identifiers");
    if (top == TOK_ID && ins == TOK_LEFT_PAR)
        error_and_die(SYNTAX_ERROR, "Expression: missing operator between identifier and '('");
    if (top == TOK_LEFT_PAR && ins == BOTTOM)
        error_and_die(SYNTAX_ERROR, "Expression: missing ')'"); 
    if (top == TOK_RIGHT_PAR && ins == TOK_ID)
        error_and_die(SYNTAX_ERROR, "Expression: missing operator between ')' and identifier"); 
    if (top == TOK_RIGHT_PAR && ins == TOK_LEFT_PAR)
        error_and_die(SYNTAX_ERROR, "Expression: missing operator between ')' and '('"); 
}

#define IS_OPERATOR(op)    (op == TOK_NOTEQ || op == TOK_LESS || op == TOK_GREATER || op == TOK_LE || op == TOK_GE || op == TOK_PLUS || op == TOK_MINUS || op == TOK_MUL || op == TOK_DIV)    


static void generate_reduction_error(expr_t topt, expr_t inst)
{
    //topt = validate_ins(topt);
    //inst = validate_ins(inst);
    
    int top = topt.type;
    int ins = inst.type; 

    if (IS_OPERATOR(top) && (ins != TOK_ID || ins != TOK_LEFT_PAR))
        error_and_die(SYNTAX_ERROR, "Expression: missing right operand");
    if (top == C && !IS_OPERATOR(ins))
        error_and_die(SYNTAX_ERROR, "Expression: missing left operand");
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

static inline expr_t get_next_token(void)
{
    getToken();
    expr_t token = validate_ins(g_lastToken);
    return token; 
}

int parse_expression(bool is_assign, bool is_condition)
{
    if (!scanner_openFile("input_test.txt"))
    {
        fprintf(stderr, "Cannot open file!\n");
        return 0;
    }

    is_it_assign = is_assign;
    bool is_it_condition = is_condition;

    bool end_of_expr = false;
    int expr_data_type = ERROR;

    stack_t     handle = stack_ctor(); // dynamic allocation
    stack_t     pda = stack_ctor();    // dynamic allocation

    expr_t     top_terminal;
    expr_t     top_terminal_tmp;
    expr_t     ins = get_next_token();

    // HACK HACK: BOTTOM equals to TOK_SPECIAL_ID, adding dollar to find out BOTTOM
    expr_t     bottom = {.type = BOTTOM, NULL};

    stack_push(&pda, bottom);
    do
    {
        top_terminal = get_top_terminal(&pda);

        if (is_it_condition && !end_of_expr && ins.type == TOK_RIGHT_PAR)
        {
            ins.type = BOTTOM;
            //ins.data.op = '$';
        }


        if (is_it_condition && ins.type == TOK_LEFT_PAR)
            end_of_expr = true;

        int result = compare_token(top_terminal.type, ins.type);     

        switch(result)
        {
                case '=':

                        if (is_it_condition) 
                            end_of_expr = false;
                        
                        handle = stack_clear(&handle);
                        handle = get_reduce_symbols(&pda, &handle);

                        if (!stack_empty(&handle))
                        {
                               if (find_right_side(&handle) == -1)
                                   if (stack_top(&pda)->type == TOK_LEFT_PAR && ins.type == TOK_RIGHT_PAR)
                                       error_and_die(SYNTAX_ERROR, "Expression: no expression between parentheses"); 
                        }
                        // push input symbol 
                        stack_push(&pda, validate_ins(g_lastToken));


                        // read next token
                        ins = get_next_token();

                        break;
                case '<':
                        // replacement of top terminal with top terminal and '<' (beginning of handle)
                        stack_add_handle_symbol(&pda, top_terminal.type);

                        // push input symbol
                        stack_push(&pda, validate_ins(g_lastToken));

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
                                //if (res != 11)
                                //    printf("%s ", tokens[res]);

                                //TODO: DONT FORGET FIRST WALK!
                                // types:
                                //  ERROR -> including bool
                                //  INTEGER
                                //  DOUBLE
                                //  STRING
                                //  ? VOID ?
                                //


                                // other combinations then these for semantic typing are ERR_SEMANTIC_TYPE
                                switch (res)
                                {
                                    // logical operators if one operand is int and second double, int is converted to double
                                    // strings are not supported
                                    //
                                    //TODO: type conversions
                                        // type conversions
                                        // int op int = int
                                        //
                                        // o1.double op double(o2.int) = double
                                        // double(o1.int) op o2.double = double
                                        // o1.double op o2.double = double
                                        // 
                                        //  
                                    case TOK_EQ:
                                        // do action
                                        expr_data_type = BOOL; 
					create_and_add_instruction(insProgram, INST_EXPR_EQUAL,0,0,0);

                                        break;
                                    case TOK_NOTEQ:
                                        expr_data_type = BOOL; 
					create_and_add_instruction(insProgram, INST_EXPR_NOT_EQ,0,0,0);
                                        break;
                                    case TOK_LESS:
                                        expr_data_type = BOOL; 
					create_and_add_instruction(insProgram, INST_EXPR_LOWER,0,0,0);
                                        break;
                                    case TOK_GREATER:
                                        expr_data_type = BOOL; 
					create_and_add_instruction(insProgram, INST_EXPR_HIGHER,0,0,0);
                                        break;
                                    case TOK_LE:
                                        expr_data_type = BOOL; 
					create_and_add_instruction(insProgram, INST_EXPR_LOW_EQ,0,0,0);
                                        break;
                                    case TOK_GE:
                                        expr_data_type = BOOL; 
					create_and_add_instruction(insProgram, INST_EXPR_HIG_EQ,0,0,0);
                                        break;
                                    case TOK_PLUS:
					create_and_add_instruction(insProgram, INST_EXPR_STR_ADD,0,0,0);
                                        // if one of the operands is string, concatenate (second operand is converted to string too, using ifj16.print)
                                        break;
                                    case TOK_MINUS:
					create_and_add_instruction(insProgram, INST_EXPR_SUB,0,0,0);
                                        break;
                                    case TOK_MUL:
					create_and_add_instruction(insProgram, INST_EXPR_MUL,0,0,0);
                                        break;
                                    case TOK_DIV:
                                        // accept only integer or double operands
                                        // int op int = integer division
                                        // otherwise default division and result is double
					create_and_add_instruction(insProgram, INST_EXPR_DIV,0,0,0);
                                        break;
                                    case TOK_ID:
					{
						// get token with attributes (TOK_ID, TOK_SPECIAL_ID, TOK_DOUBLECONST, TOK_LITERAL, TOK_CONST)
						expr_t var = handle.elem[0]; 

						// generate push
						create_and_add_instruction(insProgram, INST_PUSH,&var.symbol->data,0,0);
					}
					break;
                                }
                            }
                            else
                               generate_reduction_error(*stack_top(&pda), ins);
                        }
                        else    
                            error_and_die(SYNTAX_ERROR, "Expression: Handle does not exist!\n");

                        break; 
                case '_': 
                        if (top_terminal.type == TOK_LEFT_PAR && ins.type == BOTTOM) 
                            error_and_die(SYNTAX_ERROR, "Expression: Missing right parenthesis!");
                        if (top_terminal.type == BOTTOM && ins.type == TOK_RIGHT_PAR) 
                            error_and_die(SYNTAX_ERROR, "Expression: Unbalanced '('");
                        generate_syntax_error(*stack_top(&pda), ins); 
                case 'E':
                        error_and_die(SYNTAX_ERROR, "Expression: Logical operators are not associative!");
            }
            top_terminal_tmp = get_top_terminal(&pda);
    }
    while (ins.type != BOTTOM || (int)top_terminal_tmp.type != BOTTOM);

    putchar('\n');
    handle = stack_dtor(&handle);
    pda = stack_dtor(&pda);

    scanner_closeFile();

    // if bool is evaluated and top-down parser does not process condition, set error as resulting type
    if (expr_data_type == BOOL && !is_condition)
        expr_data_type = ERROR;

    return expr_data_type;
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
            stack_push(&string, stack->elem[end]);
        }
        else
        {
            long str_end = string.top;
            long str_start = 0;

            for (; str_start < str_end; str_end--, str_start++)
            {
                expr_t tmp = string.elem[str_start];
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
       expr_t left = {.type = left_side}; 
       stack->top = end-1;
       stack_push(stack, left); 
    }
}

int find_right_side(const stack_t *const handle)
{
    if (handle == NULL)
        return -1;

    const int   *rule;
    expr_t     *top_terminal;
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

expr_t get_top_terminal(stack_t *stack) 
{ 

    long top = stack->top;
    obj_t cur_token;

    for (; top != -1; top--)
    {
        cur_token = stack->elem[top];

        if ((int)cur_token.type >= TOK_EQ && cur_token.type <= BOTTOM)
            return cur_token;
    }
    return cur_token;
}

stab_t* staticSym = NULL;

instruction_list_t* insProgram = NULL;
int main(void)
{

    insProgram = init_inst_list();
    fprintf(stderr, "\n==========\nResult type: %i\n", parse_expression(false, false));
    inst_list_print(insProgram);
	return 0;
}
