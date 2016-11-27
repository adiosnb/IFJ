#define OP_PARSER
#include "op-parser.h"
#include <stdio.h>

#include "../scanner.h"
#include "utils/dynamic_stack.h"
#include "../error.h"
#include "../instruction_list.h"
#include "../ial.h"

//#define NDEBUG

extern const char op_table[][MAX_TERMINALS];
extern const int rule_table[][MAX_RULES];
extern const int rule_len[MAX_RULES];
extern stab_t* staticSym;
extern instruction_list_t* insProgram;
extern char* parser_class;
extern char* parser_fun;


bool is_it_assign = false;

bool shouldGenerate = false;

int global_type = INTEGER;

// the count of )
int rightParentCount = 0;

data_t* semantic_transform(dstack_t* symbols, int res);

data_t* token2symbol()
{
	data_t* res = NULL;
	data_t dt;
	switch(getLastToken())
	{
		case TOK_ID:
			res = stable_search_variadic(staticSym, 3, parser_class,parser_fun, getTokString());
			if(!res)
				res = stable_search_variadic(staticSym, 2, parser_class,getTokString());
			if(!res)
				error_and_die(SEMANTIC_ERROR, "Missing symbol '%s'", getTokString());
			break;
		case TOK_SPECIAL_ID:
			res = stable_search_variadic(staticSym, 1,getTokString());
			if(!res)
				error_and_die(SEMANTIC_ERROR, "Missing symbol '%s'", getTokString());
			break;
		default:
			switch(getLastToken())
			{
				case TOK_CONST:
					dt.type = INTEGER;
					dt.data.arg_type = INTEGER;
					dt.data.data.i = getTokInt();
					break;
				case TOK_DOUBLECONST:
					dt.type = DOUBLE;
					dt.data.arg_type = DOUBLE;
					dt.data.data.d = getTokDouble();
					break;
				case TOK_LITERAL:
					dt.type = STRING;
					dt.data.arg_type = STRING;
					dt.data.data.s = str_init();
					str_append_chars(&dt.data.data.s, getTokString());
					break;
				default:
					return NULL;
			}
			static int counter = 0;
			char buff[100];
			sprintf(buff,"%u", counter++);
			res = stable_add_variadic(staticSym, dt, 2, "ifj16.op_const",buff);
			if(res == NULL)
				error_and_die(INTERNAL_ERROR, "Failed to create constant");
	}

	if(!res)
		error_and_die(INTERNAL_ERROR,"Cyka blyat");
	return res;
}

static inline expr_t validate_ins(t_token s)
{
    expr_t exp;
    exp.type = s.type;
    // just for now to see where is end of input tokens string
   
    if (s.type == TOK_RIGHT_PAR)
	rightParentCount--;	
    if (s.type == TOK_LEFT_PAR) 
	rightParentCount++;

    if ((is_it_assign && exp.type == TOK_DELIM)
	|| (!is_it_assign && exp.type == TOK_RIGHT_PAR && rightParentCount == -1))
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

    if(shouldGenerate)
	exp.symbol = token2symbol();
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

void print_dstack(dstack_t *stack)
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

int parse_expression(bool should_generate, bool is_condition)
{
    rightParentCount = 0;
    shouldGenerate = should_generate;
    is_it_assign = !is_condition;
    bool is_it_condition = is_condition;

    bool end_of_expr = false;
    int expr_data_type = ERROR;

    dstack_t     handle = dstack_ctor(); // dynamic allocation
    dstack_t     pda = dstack_ctor();    // dynamic allocation

    expr_t     top_terminal;
    expr_t     top_terminal_tmp;
    expr_t     ins = get_next_token();

    // HACK HACK: BOTTOM equals to TOK_SPECIAL_ID, adding dollar to find out BOTTOM
    expr_t     bottom = {.type = BOTTOM, NULL};

    dstack_push(&pda, bottom);
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
                        
                        handle = dstack_clear(&handle);
                        handle = get_reduce_symbols(&pda, &handle);

                        if (!dstack_empty(&handle))
                        {
                               if (find_right_side(&handle) == -1)
                                   if (dstack_top(&pda)->type == TOK_LEFT_PAR && ins.type == TOK_RIGHT_PAR)
                                       error_and_die(SYNTAX_ERROR, "Expression: no expression between parentheses"); 
                        }
                        // push input symbol 
                        dstack_push(&pda, validate_ins(g_lastToken));


                        // read next token
                        ins = get_next_token();

                        break;
                case '<':
                        // replacement of top terminal with top terminal and '<' (beginning of handle)
                        dstack_add_handle_symbol(&pda, top_terminal.type);

                        // push input symbol
                        dstack_push(&pda, validate_ins(g_lastToken));

                        // read next token
                        ins = get_next_token();

                        break;
                case '>':
                        handle = dstack_clear(&handle);
                        handle = get_reduce_symbols(&pda, &handle);

                        if (!dstack_empty(&handle))
                        {
                            int res = find_right_side(&handle);

                            if (res != -1)
                            {
                                const int *left_side = get_rule(res);
                                
			        // use symbols to calculate new type
				data_t* op_type = NULL;
				if(should_generate)
				       op_type = semantic_transform(&pda,res);

                                // switch topmost pda rule string with left side
                                dstack_reduce_rule(&pda, *left_side); 

				// set new type
				pda.elem[pda.top].symbol = op_type;


				expr_t top = get_top_terminal(&pda);
                                if (expr_data_type == BOOL && top.type != BOTTOM)
					error_and_die(SEMANTIC_TYPE_ERROR, "Conversion of bool to int");

				// use handle to detect types

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
                                //
				//if(shouldGenerate)
				//	printf("Typ: %d\n",op_type->type);

				// if code generation is switched off, then continue
                                if(!shouldGenerate)
					break;
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
					
					//TODO choose correct operation
					if(op_type->type == STRING)
						create_and_add_instruction(insProgram, INST_EXPR_STR_ADD,0,0,0);
					else 
						create_and_add_instruction(insProgram, INST_EXPR_ADD,0,0,0);
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

						if(var.symbol == NULL)
							error_and_die(INTERNAL_ERROR, "Missing symbol.");
						if(var.symbol->data.arg_type == INSTRUCTION)
							error_and_die(SEMANTIC_TYPE_ERROR, "Expected a variable symbol in expression.");
						// generate push
						create_and_add_instruction(insProgram, INST_PUSH,&var.symbol->data,0,0);
						if(global_type < var.symbol->type)
							global_type = var.symbol->type;
					}
					break;
                                }
                            }
                            else
                               generate_reduction_error(*dstack_top(&pda), ins);
                        }
                        else    
                            error_and_die(SYNTAX_ERROR, "Expression: Handle does not exist!\n");

                        break; 
                case '_': 
                        if (top_terminal.type == TOK_LEFT_PAR && ins.type == BOTTOM) 
                            error_and_die(SYNTAX_ERROR, "Expression: Missing right parenthesis!");
                        if (top_terminal.type == BOTTOM && ins.type == TOK_RIGHT_PAR) 
                            error_and_die(SYNTAX_ERROR, "Expression: Unbalanced '('");
                        generate_syntax_error(*dstack_top(&pda), ins); 
                case 'E':
                        error_and_die(SEMANTIC_TYPE_ERROR, "Expression: Logical operators are not associative!");
            }
            top_terminal_tmp = get_top_terminal(&pda);
    }
    while (ins.type != BOTTOM || (int)top_terminal_tmp.type != BOTTOM);

    int expr_glob_type = 0;
    if(shouldGenerate)
    {
	expr_glob_type = pda.elem[pda.top].symbol->type;
    }
    
    handle = dstack_dtor(&handle);
    pda = dstack_dtor(&pda);


    // if bool is evaluated and top-down parser does not process condition, set error as resulting type
    if( shouldGenerate)
    {
   	if (expr_data_type == BOOL && !is_condition)
		error_and_die(SEMANTIC_TYPE_ERROR, "Expected a assign expression");
    	if (is_condition && expr_data_type != BOOL)
		error_and_die(SEMANTIC_TYPE_ERROR, "Expected a relation expression");
    }
    if(is_condition)
	return expr_data_type;
    return expr_glob_type;
}

static inline bool is_dstack_bottom(const dstack_t *const stack)
{
    if (stack != NULL)
        return stack->top == 0 && stack->elem[0].type == BOTTOM;
    return true;
}

dstack_t get_reduce_symbols(const dstack_t *const stack, dstack_t *const handle)
{
    dstack_t string = *handle; 

    if (stack == NULL )
        return string;

    for (long end = stack->top; !is_dstack_bottom(stack); end--)
    {
        if (stack->elem[end].type != '<')
        {
            dstack_push(&string, stack->elem[end]);
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

void dstack_reduce_rule(dstack_t *const stack, int left_side)
{
    if (stack == NULL)
        return;

    long end; 

    for (end = stack->top; !is_dstack_bottom(stack); end--)
    {
       if (stack->elem[end].type == '<')
          break; 
    }

    if (stack->elem[end].type == '<')
    {
       expr_t left = {.type = left_side}; 
       stack->top = end-1;
       dstack_push(stack, left); 
    }
}

int find_right_side(const dstack_t *const handle)
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

        if (dstack_elem_count(handle) != left_side_len) 
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

expr_t get_top_terminal(dstack_t *stack) 
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

// try to reduce semantic types and thus prove semantic correctnes
data_t* semantic_transform(dstack_t* symbols, int res)
{
	expr_t opA = symbols->elem[symbols->top - rule_sempos[res][0]];	
	expr_t opB = symbols->elem[symbols->top - rule_sempos[res][1]];	

	int typeA = opA.symbol->type;
	int typeB = opB.symbol->type;
	//printf("A: %d B: %d\n",typeA, typeB);

	data_t* rule = opA.symbol;
	switch(res)
	{
		case TOK_EQ:
		case TOK_LE:	
		case TOK_GE:	
		case TOK_GREATER:
		case TOK_LESS:
		case TOK_NOTEQ:
		case TOK_MINUS:
		case TOK_DIV:
		case TOK_MUL:
			if(typeA == STRING || typeB == STRING)
				error_and_die(SEMANTIC_TYPE_ERROR,"Forbidden conversion of string");				
			if(typeB == DOUBLE)
				rule= opB.symbol;
		case TOK_PLUS:
			if(typeB > typeA)
				rule= opB.symbol;
			break;	
		case 11:
		case TOK_ID:
			break;
		default:
			fprintf(stderr,"Unk operation in op parser\n");
	}
	return rule;
}
