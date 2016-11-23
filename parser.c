//#define	DEBUG 2 
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "stable.h"
#include "error.h"


stab_t	*staticSym = NULL;
instruction_list_t* insProgram	= NULL;
instruction_list_t* insInit	= NULL;

char*	parser_class = NULL;
char*	parser_fun = NULL;

int	isTokenKeyword(int kw)
{
	return (getLastToken() == TOK_KEYWORD && getTokInt() == kw);
}

int	isIdentifier()
{
	return (getLastToken() == TOK_ID
		||  getLastToken() == TOK_SPECIAL_ID);
}

int	isTokenTypeSpecifier()
{
	return (getLastToken() == TOK_KEYWORD && (
		getTokInt() == KW_VOID ||
		getTokInt() == KW_INT ||
		getTokInt() == KW_DOUBLE||
		getTokInt() == KW_STRING));
}

/******************************************************************************
 			SEMANTIC UTILS
******************************************************************************/

void generateFunctionCall(data_t* func,data_t* retSym)
{
	// memory address to receive the return value
	argument_var_t* retVal = (retSym != NULL)?(&retSym->data):NULL;
	if(!func)
		return;
	switch(func->data.data.i)
	{
		case BUILTIN_READ:
			switch(func->type)
			{
				case INTEGER:
					create_and_add_instruction(insProgram, INST_READ_INT, retVal,0,0);
					break;
				case DOUBLE:
					create_and_add_instruction(insProgram, INST_READ_DOUBLE, retVal,0,0);
					break;
				case STRING:
					create_and_add_instruction(insProgram, INST_READ_STRING, retVal,0,0);
					break;
			}
			break;
		case BUILTIN_LEN:
			create_and_add_instruction(insProgram, INST_CALL_LEN, retVal,0,0);
			break;
		case BUILTIN_SUBSTR:
			create_and_add_instruction(insProgram, INST_CALL_SUBSTR, retVal,0,0);
			break;
		case BUILTIN_CMP:
			create_and_add_instruction(insProgram, INST_CALL_CMP, retVal,0,0);
			break;
		case BUILTIN_FIND:
			create_and_add_instruction(insProgram, INST_CALL_FIND, retVal,0,0);
			break;
		case BUILTIN_SORT:
			create_and_add_instruction(insProgram, INST_CALL_SORT, retVal,0,0);
			break;
			
		default:
			create_and_add_instruction(insProgram, INST_CALL, func->data.data.instruction,retVal,0);
	}
}


void fillClassData(data_t* data)
{
	data->type = INTEGER;
	data->data.arg_type = INTEGER;	
	data->data.data.i = PLACEHOLDER_CLASS;
}

void fillFunctionData(data_t* data,int type)
{
	data->type = type;
	data->data.arg_type = INSTRUCTION;	
	data->data.data.instruction = NULL; 
	data->next_param = NULL;
}
void fillStaticVarData(data_t* data,int type)
{
	data->type = type;
	data->data.arg_type = type;	
	data->data.data.i = UNINITIALIZED;
	data->next_param = NULL;
}

void inicializeData(data_t* data)
{
	if(data->type == STRING)
	{	
		data->data.data.s = str_init();
	}
}

void fillLocalVarData(data_t* data,int type, int stackPos)
{
	data->type = type;
	data->data.arg_type = STACK_EBP;	
	data->data.data.i = stackPos;
	data->next_param = NULL;
}

data_t* createConstant(int type, int iVal, double dVal, char* cVal)
{
	static int constNum = 0;
	char buffer[255];
	snprintf(buffer,254,"%d",constNum++);

	data_t const_data;
	fillStaticVarData(&const_data,type);

	switch(type)
	{
		case INTEGER:
			const_data.data.data.i = iVal;
			break;
		case DOUBLE:
			const_data.data.data.d = dVal;
			break;
		case STRING:
			inicializeData(&const_data);
			str_append_chars(&const_data.data.data.s, cVal);
			break;
				
	}
	return stable_add_variadic(staticSym, const_data,2, "ifj16.const", buffer);
}

data_t* generateLabel(instruction_list_t* list)
{
	static int labelCounter = 0;

	instruction_item_t* itemLabel = create_and_add_instruction(list, INST_LABEL, 0,0,0);

	data_t dataLbl;
	fillFunctionData(&dataLbl,VOID);
	dataLbl.data.data.instruction = itemLabel;	

	char buff[100];
	snprintf(buff,99,"%u", labelCounter++);
	return stable_add_variadic(staticSym,dataLbl, 3, "ifj16", "labels", buff);
}
void addBuiltInToTable(stab_t* table)
{
	data_t data, *ptr;
	// class IFJ16
	fillClassData(&data);
	stable_add_var(table, "ifj16",data);
	// print 
	fillFunctionData(&data,VOID);
	data.data.data.i = BUILTIN_PRINT;
	stable_add_variadic(table,data,2,"ifj16","print");
	// readInt 
	fillFunctionData(&data,INTEGER);
	data.data.data.i = BUILTIN_READ;
	stable_add_variadic(table,data,2,"ifj16","readInt");
	// readDouble
	fillFunctionData(&data,DOUBLE);
	data.data.data.i = BUILTIN_READ;
	stable_add_variadic(table,data,2,"ifj16","readDouble");
	// readString
	fillFunctionData(&data,STRING);
	data.data.data.i = BUILTIN_READ;
	stable_add_variadic(table,data,2,"ifj16","readString");
	 
	// length 
	fillLocalVarData(&data,INTEGER, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","length","s");

	fillFunctionData(&data,INTEGER);
	data.data.data.i = BUILTIN_LEN;
	data.next_param = ptr;
	stable_add_variadic(table,data,2,"ifj16","length");

	// TODO: substr, compare, find
	// substr
	fillLocalVarData(&data,INTEGER, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","substr","n");
	fillLocalVarData(&data,INTEGER, -3);
	data.next_param = ptr;
	ptr =  stable_add_variadic(table,data,3,"ifj16","substr","i");
	fillLocalVarData(&data,STRING, -4);
	data.next_param = ptr;
	ptr =  stable_add_variadic(table,data,3,"ifj16","substr","s");
	fillFunctionData(&data,STRING);
	data.next_param = ptr;
	data.data.data.i = BUILTIN_SUBSTR;
	stable_add_variadic(table,data,2,"ifj16","substr");

	//compare 
	fillLocalVarData(&data,STRING, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","compare","s2");
	fillLocalVarData(&data,STRING, -3);
	data.next_param = ptr;
	ptr =  stable_add_variadic(table,data,3,"ifj16","compare","s1");
	fillFunctionData(&data,INTEGER);
	data.next_param = ptr;
	data.data.data.i = BUILTIN_CMP;
	stable_add_variadic(table,data,2,"ifj16","compare");
	// find
	fillLocalVarData(&data,STRING, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","find","s");
	fillLocalVarData(&data,STRING, -3);
	data.next_param = ptr;
	ptr =  stable_add_variadic(table,data,3,"ifj16","find","search");
	fillFunctionData(&data,INTEGER);
	data.next_param = ptr;
	data.data.data.i = BUILTIN_FIND;
	stable_add_variadic(table,data,2,"ifj16","find");

	//sort 
	fillLocalVarData(&data,STRING, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","sort","s");
	fillFunctionData(&data,STRING);
	data.next_param = ptr;
	data.data.data.i = BUILTIN_SORT;
	stable_add_variadic(table,data,2,"ifj16","sort");



}

char* type2str(int type)
{
	static char* str[] = {"INTEGER","DOUBLE","STRING",NULL,NULL,NULL,"VOID"};
	return str[type];
}

// Set correct stack positions for function formal parameters
void util_correctParamList(data_t* func)
{
	data_t* ptr = func->next_param;
	if(!ptr)
		return;
	int offset = 0;
	while(ptr)
	{
		if(ptr->next_param == NULL)
			offset = ptr->data.data.i + 2;	
		ptr = ptr->next_param;
	}
	
	ptr = func->next_param;
	while(ptr)
	{
		ptr->data.data.i -= offset;
		ptr = ptr->next_param;
	}
}

int class_definition_list();
int type_specifier(int* type);
int iteration_statement();
int jump_statement();
int definition();
int block_items_list();
int compound_statement();
int function_parameters_list();
int statement();
int parameter_definition();
int function_arguments_list(data_t**);

/*<expr>                         -> expression
<expr>                         -> eps
*/
/*
<type-specifier>               -> String
<type-specifier>               -> int
<type-specifier>               -> double
<type-specifier>               -> void
*/
int type_specifier(int* out_type)
{
	getToken();
	switch(getTokInt())
	{
		default:
			return SYN_ERR;
		case KW_INT:
			*out_type = INTEGER;
			break;
		case KW_DOUBLE:
			*out_type = DOUBLE;
			break;
		case KW_VOID:
			*out_type = VOID;
			break;
		case KW_STRING:
			*out_type = STRING;
			break;
	}
	return SYN_OK;	
}

/*
<identifier>                   -> floating-point-constant
<identifier>                   -> decimal-constant
<identifier>                   -> fully-qualified-identifier
<identifier>                   -> simple-identifier
*/

int expression()
{
	while(1)
	{
		int state = getToken();
		switch(state)
		{
			case TOK_DELIM:
			case TOK_RIGHT_PAR:
				ungetToken();
				return SYN_OK;
			default:
				break;
		}
	}
	return SYN_OK;
}

int builtin_print();

//<more-next>                    -> expression 
//<more-next>                    -> <identifier> ( <function-parameters-list> ) 
int more_next(data_t* var)
{

	getToken();
	if(isIdentifier() && getToken() == TOK_LEFT_PAR)
	{
		ungetToken();
		int isPrint = !strcmp(getTokString(),"ifj16.print");
		// is it a ID = ifj16.print() case ?
		if(isPrint)
		{
			getToken();
			builtin_print();
			// NOTE: it's a semantic error in any case, just check syntax
			error_and_die(SYNTAX_ERROR,"Assigned void function to variable.");
		} else 
		{
			char* callName = getTokString();
			data_t*	func = NULL, *data = NULL;
			if(isSecondPass)
			{
				if(getLastToken() == TOK_ID)
					func = stable_search_variadic(staticSym, 2, parser_class, getTokString());
				else
					func = stable_search_variadic(staticSym, 1, getTokString());
				if(!func)
					error_and_die(SEMANTIC_ERROR,"Function '%s' not found.",getTokString());
				// TODO: verify ERROR code in this special case
				if(func->data.arg_type != INSTRUCTION)
					error_and_die(SEMANTIC_ERROR,"Expecting '%s' to be function", getTokString());
				if(func->type != var->type)
					error_and_die(SEMANTIC_TYPE_ERROR,"Function call type dismatch [%s]",callName);
				data = func->next_param;
			}
			getToken();

			if(function_arguments_list(&data) == SYN_ERR)
				return SYN_ERR;

			if(data != NULL)
				error_and_die(SEMANTIC_TYPE_ERROR,"Too few arguments in funciton call [%s]",callName);

			if(getToken() != TOK_RIGHT_PAR)
				error_and_die(SYNTAX_ERROR,"Expected )");
			// generate function call
			if(isSecondPass)
			{
				// TODO switch for built in
				generateFunctionCall(func,var);
			}
		}
		return SYN_OK;
	} else {
		ungetToken();
		ungetToken();
		if(expression() == SYN_ERR)
			return SYN_ERR;
		//TODO var->type compare 
		if(isSecondPass)
			create_and_add_instruction(insProgram, INST_STORE, &var->data, 0,0);
		GEN("Verify if result of expr can be assigned. If do, generate assign");
	}	
	return SYN_OK;
}
// builtin_print is a special case of function call which supports
// a concatenation as an argument
int builtin_print()
{
	int paramCount = 0;
	if(getToken() == TOK_RIGHT_PAR)
		error_and_die(SYNTAX_ERROR,"Expected a term or concatenation");
	ungetToken();	
	do {
		data_t* var = NULL;
		int res = getToken();
		if(isSecondPass)
		{
			switch(res)
			{
				case TOK_ID:
					var = stable_search_variadic(staticSym, 3, parser_class,parser_fun,getTokString());
					if(!var)
						var = stable_search_variadic(staticSym, 2, parser_class,getTokString());
					if(!var)
						error_and_die(SEMANTIC_ERROR, "Undefined variable '%s'",getTokString());
					break;
				case TOK_SPECIAL_ID:
					var = stable_search_variadic(staticSym,1, getTokString());
					if(!var)
						error_and_die(SEMANTIC_ERROR, "Undefined variable '%s'",getTokString());
					break;	
				case TOK_CONST:
					var = createConstant(INTEGER, getTokInt(), 0,NULL);
					break;
				case TOK_DOUBLECONST:
					var = createConstant(DOUBLE, 0, getTokDouble(),NULL);
					break;
				case TOK_LITERAL:
					var = createConstant(STRING, 0, 0,getTokString());
					break;
				default:
					error_and_die(SYNTAX_ERROR,"Expected an identifier or a constant.");
			}
			
			if(!var)
				error_and_die(INTERNAL_ERROR,"Failed to create constant");

			// now generate PUSH
			create_and_add_instruction(insProgram, INST_PUSH, &var->data,0,0);
			paramCount++;
		} else {
			switch(res)
			{
				case TOK_ID:
				case TOK_SPECIAL_ID:
				case TOK_CONST:
				case TOK_DOUBLECONST:
				case TOK_LITERAL:
					break;
				default:
					error_and_die(SYNTAX_ERROR,"Expected an identifier or a constant.");
			}
		}
		
		res = getToken();
		if(res == TOK_RIGHT_PAR)
			break;
		if(res != TOK_PLUS)
			error_and_die(SYNTAX_ERROR,"Expected +");

	} while (1);
	if(isSecondPass)
	{
		data_t* varCount = createConstant(INTEGER, paramCount, 0,NULL);
		create_and_add_instruction(insProgram, INST_CALL_PRINT, &varCount->data,0,0);
	}
	if(getToken() != TOK_DELIM)
		error_and_die(SYNTAX_ERROR,"Expected ;");

	return SYN_OK;
	
}


//<next>                         -> = <more-next>
//<next>                         -> ( <function-parameters-list> ) 

int next(data_t* symbol,char* id)
{	
	int isPrint = !strcmp(getTokString(),"ifj16.print");
	switch(getToken())
	{
		case TOK_ASSIGN:
			if(more_next(symbol) == SYN_ERR)
				return SYN_ERR;
			if(getToken() != TOK_DELIM)
				error_and_die(SYNTAX_ERROR,"Expected ;");
			return SYN_OK;
		case TOK_LEFT_PAR:
			if(isPrint)
			{
				return builtin_print();
			} else {
				GEN("Check if symbol '%s' is defined", getTokString());

				data_t*	dt = NULL;
				if(isSecondPass)
					dt = symbol->next_param;
				if(function_arguments_list(&dt) == SYN_ERR)
					return SYN_ERR;	
				if(dt != NULL)
					error_and_die(SEMANTIC_TYPE_ERROR,"Too few arguments in funciton call [%s]",id);
				GEN("Generate a function call INSTR");
				if(getToken() != TOK_RIGHT_PAR)
					error_and_die(SYNTAX_ERROR,"Expected )");
				if(getToken() != TOK_DELIM)
					error_and_die(SYNTAX_ERROR,"Expected ;");
				return SYN_OK;
			}
	}
	error_and_die(SYNTAX_ERROR,"Expected = or (");
	return SYN_ERR;
}


//<jump-statement>               -> return <expression> ;
int jump_statement()
{
	getToken();
	if(!isTokenKeyword(KW_RETURN))
		error_and_die(SYNTAX_ERROR,"Expected return");
	
	int hasExpression = 1;
	if(getToken() == TOK_DELIM)
		hasExpression = 0;
	ungetToken();

	data_t* fn = stable_search_variadic(staticSym, 2, parser_class, parser_fun);
	if(isSecondPass)
	{
		if(!fn)
			error_and_die(INTERNAL_ERROR,"Can't get the handle of function");
		if(fn->type == VOID && hasExpression)
		{
			error_and_die(SEMANTIC_TYPE_ERROR, "Void function can't return an expr");
		}
		if(fn->type != VOID && !hasExpression)
		{
			error_and_die(RUNTIME_UNINITIALIZED, "Missing return value");
		} 
	}

	if(expression() == SYN_ERR)
		return SYN_ERR;

	if(isSecondPass)
	{
		if(fn->type == VOID)
		{
			create_and_add_instruction(insProgram,INST_RET, 0,0,0);
		} else {
			//TODO 
			create_and_add_instruction(insProgram,INST_RET, 0xDEAFBEEF,0,0);
		}
	}

	if(getToken() != TOK_DELIM)
		error_and_die(SYNTAX_ERROR,"Expected ; at the end of return statement.");

	GEN("Generate RETURN instruction and expression");

	return SYN_OK;

}

//<iteration-statement>          -> while ( expression ) <compound-statement>

int iteration_statement()
{
	instruction_item_t* skipJump = NULL; 
	data_t* lblTest = NULL;

	getToken();
	if(!isTokenKeyword(KW_WHILE))
		error_and_die(SYNTAX_ERROR,"Expected return");

	GEN("Generate WHILE.test label"); 
	if(isSecondPass)
	{
		lblTest= generateLabel(insProgram);
	}
	if(getToken() != TOK_LEFT_PAR)
		error_and_die(SYNTAX_ERROR,"Expected (");

	if(expression() == SYN_ERR)
		return SYN_ERR;

	if(getToken() != TOK_RIGHT_PAR)
		error_and_die(SYNTAX_ERROR,"Expected )");

	if(isSecondPass)
	{
		skipJump = create_and_add_instruction(insProgram, INST_JZ, NULL, 0xDEADBEEF,NULL); 
		GEN("Generate COMPARE and JUMP test"); 
	}
	if(compound_statement() == SYN_ERR)
		return SYN_ERR;
	if(isSecondPass)
	{
		GEN("Generate WHILE_SKIP label and altern CMP jmp addres");
		create_and_add_instruction(insProgram, INST_JMP, &lblTest->data,0,0);
		data_t* lblSkip = generateLabel(insProgram);

		// now altern compare to jmp to skip
		skipJump->instruction.addr1 = &lblSkip->data;
	}
	return SYN_OK;
}

//<selection-statement>          -> if ( expression ) <compound-statement> else <compound-statement>
int selection_statement()
{
	instruction_item_t* selection = NULL;
	instruction_item_t* endJmp = NULL;
	data_t* lblElse = NULL;
	data_t* lblSkip = NULL;

	getToken();
	if(isTokenKeyword(KW_IF))
	{
		if(getToken() != TOK_LEFT_PAR)
			error_and_die(SYNTAX_ERROR,"Expected (");
		
		if(expression() == SYN_ERR)
			return SYN_ERR;

		if(isSecondPass)
		{
			GEN("Generate a CMP and JUMP.");
			selection = create_and_add_instruction(insProgram, INST_JNZ, 0,0,0);
		}
		if(getToken() != TOK_RIGHT_PAR)
			error_and_die(SYNTAX_ERROR,"Expected )");
	
		
		if(compound_statement() == SYN_ERR)
			return SYN_ERR;		

		if(isSecondPass)
		{
			GEN("Generate a JMP.");
			// jump to the end of IF-ELSE
			endJmp = create_and_add_instruction(insProgram, INST_JMP, 0,0,0);
			GEN("Generate a IF_ELSE label and altern JMP");
			lblElse = generateLabel(insProgram);
			// altern the jmp after condition to jmp to ELSE branch
			selection->instruction.addr1 = &lblElse->data;
		}

		getToken();
		if(!isTokenKeyword(KW_ELSE))
			error_and_die(SYNTAX_ERROR,"Expected else");

		if(compound_statement() == SYN_ERR)
			return SYN_ERR;		
		
		if(isSecondPass)
		{
			GEN("Generate a IF_SKIP label and altern JMP.");
			lblSkip = generateLabel(insProgram);
			// altern JMP after first compound to jmp to the end
			endJmp->instruction.addr1 = &lblSkip->data;
		}
		return SYN_OK;	
		
	}	
	error_and_die(SYNTAX_ERROR,"Expected IF");
	return SYN_ERR;
}

//<assign-statement>             -> identifier <next> ;
int assign_statement()
{
	getToken();
	if(isIdentifier())
	{	
		char* assign_id = getTokString();
		data_t* symbol = NULL;
		GEN("Check if variable '%s'  is defined", getTokString());

		if(isSecondPass)
		{
			if(getLastToken() == TOK_ID)
				symbol = stable_search_variadic(staticSym,3,parser_class,parser_fun,getTokString());
			else
				symbol = stable_search_variadic(staticSym,1,getTokString());
			if(!symbol)
				error_and_die(SEMANTIC_ERROR,"Symbol %s is missing",getTokString());
		}
		return next(symbol,assign_id);
	}
	error_and_die(SYNTAX_ERROR,"Expected identifier or full-identifier.");
	return SYN_ERR;
}
//<compound-statement>           -> { <block-items-list> }
int compound_statement()
{
	if(getToken() != TOK_LEFT_BRACE)
		error_and_die(SYNTAX_ERROR,"Expected {");

	if(block_items_list() == SYN_ERR)
		return SYN_ERR;

	if(getToken() != TOK_RIGHT_BRACE)
		error_and_die(SYNTAX_ERROR,"Expected {");
	return SYN_OK;
}


//<block-item>                   -> <definition>
//<block-item>                   -> <statement>
int block_item()
{
	getToken();
	if(isTokenKeyword(KW_STATIC) || isTokenTypeSpecifier())
	{	
		ungetToken(); 
		return definition();
	} else {
		ungetToken();
		int res = statement();
		return res;
			
	}
	return SYN_ERR;
}

//<block-items-list>             -> <block-item> <block-items-list>
//<block-items-list>             -> eps
int block_items_list()
{
	int type = getToken();
	if(type == TOK_EOF)
		error_and_die(SYNTAX_ERROR,"Missing }");
	if(type == TOK_RIGHT_BRACE)
	{
		ungetToken();
		return SYN_OK;
	}
	ungetToken();
	
	if(block_item() == SYN_ERR)	
		return SYN_ERR;

	return block_items_list();
}

//<statement>                    -> <jump-statement>
//<statement>                    -> <iteration-statement>
//<statement>                   -> <selection-statement>
//<statement>                   -> <assign-statement>
//<statement>                    -> <compound-statement>

int statement()
{
	int type = getToken();
	int val = getTokInt();
	ungetToken();
	switch(type)
	{
		case TOK_KEYWORD:
		{
			switch(val)
			{
				case KW_RETURN:
					return jump_statement();
				case KW_IF:
					return selection_statement();
				case KW_WHILE:
					return iteration_statement();
				default:
					error_and_die(SYNTAX_ERROR,"Got keyword, Expected if, while or return");
			}
		}
		case TOK_ID:
		case TOK_SPECIAL_ID:
			return assign_statement();	
		case TOK_LEFT_BRACE:
			return compound_statement();
		default:
			break;
	}
	error_and_die(SYNTAX_ERROR,"Expected a statement");
	return SYN_ERR;
}

//<argument-definition>	-> <term>
int argument_definition(data_t** fun)
{
	data_t*	var;
	// payload is used to inicialize constants
	data_t payload;
	if(isSecondPass)
	{
		if(!(*fun))
			error_and_die(SYNTAX_ERROR,"Semantic error: too many arguments");
		switch(getToken())
		{
			case TOK_ID:
				var = stable_search_variadic(staticSym, 3, parser_class, parser_fun,getTokString());
				if(!var)
					var = stable_search_variadic(staticSym,2, parser_class,getTokString());
				if(!var)
					error_and_die(SEMANTIC_ERROR," '%s' is missing.", getTokString());
				break;
			case TOK_SPECIAL_ID:
				var = stable_search_variadic(staticSym, 2, parser_class ,getTokString());
				if(!var)
					error_and_die(SEMANTIC_ERROR,"'%s' is an undefined symbol.", getTokString());
				break;
			case TOK_LITERAL:
				//TODO: fix duplicates
				var = createConstant(STRING, 0,0, getTokString());

				/*	fillStaticVarData(&payload, STRING);
				inicializeData(&payload);
				str_append_chars(&payload.data.data.s, getTokString());

				var = stable_add_variadic(staticSym,payload, 3, "ifj16","const",getTokString());
				*/
				break;
			case TOK_CONST:
				var = createConstant(INTEGER, getTokInt(),0,0);
				/*
				fillStaticVarData(&payload, INTEGER);
				payload.data.data.i = getTokInt();

				var = stable_add_variadic(staticSym,payload, 3, "ifj16","const",atoi(getTokInt()));
				*/
				break;
			case TOK_DOUBLECONST:
				var = createConstant(DOUBLE, 0,getTokDouble(),0);
				/*fillStaticVarData(&payload, DOUBLE);
				payload.data.data.d = getTokDouble();

				var = stable_add_variadic(staticSym,payload,3, "ifj16","const",atof(getTokDouble()));
				*/
				break;
			default:
				error_and_die(SYNTAX_ERROR,"Expected a term in function call");
		}
		if(!var)
			error_and_die(INTERNAL_ERROR, "Failed to create constant");

		// push variable
		create_and_add_instruction(insProgram, INST_PUSH, &var->data,0,0);	

		if(var->type != (*fun)->type)
			error_and_die(SYNTAX_ERROR,"SEM - argument type dismatch.");
		*fun = (*fun)->next_param;
	} else {
		switch(getToken())
		{
			case TOK_ID:
			case TOK_SPECIAL_ID:
			case TOK_LITERAL:
			case TOK_CONST:
			case TOK_DOUBLECONST:
				break;
			default:
				error_and_die(SYNTAX_ERROR,"Expected a term in function call");
		}
	}
	return SYN_OK;
}
//<more-function-arguments>     -> eps
//<more-function-arguments>     -> , <argument-declaration> <more-function-arguments> 

int more_function_arguments(data_t** fun)
{
	switch(getToken())
	{
		case TOK_LIST_DELIM:
				if(argument_definition(fun) == SYN_ERR)
					return SYN_ERR;
				return more_function_arguments(fun);
		case TOK_RIGHT_PAR:
			ungetToken();
			return SYN_OK;
		default:
			error_and_die(SYNTAX_ERROR,"Expected ,");
	}
	return SYN_ERR;
			
}

//<function-arguments-list>     -> eps
//<function-arguments-list>     -> <argument-definition> <more-function-arguments>

int function_arguments_list(data_t** fun)
{
	if(getToken() == TOK_RIGHT_PAR)
	{
		ungetToken();
		return SYN_OK;
	} else {
		ungetToken();

		if(argument_definition(fun) == SYN_ERR)
			return SYN_ERR;
		return more_function_arguments(fun);
	}
}

data_t** last_param;
int	param_count;

//<more-function-parameters>     -> eps
//<more-function-parameters>     -> , <parameter-declaration> <more-function-parameters> 

int more_function_parameters()
{
	switch(getToken())
	{
		case TOK_LIST_DELIM:
				if(parameter_definition() == SYN_ERR)
					return SYN_ERR;
				return more_function_parameters();
		case TOK_RIGHT_PAR:
			ungetToken();
			return SYN_OK;
		default:
			error_and_die(SYNTAX_ERROR,"Expected ,");
	}
	return SYN_ERR;
			
}

//<function-parameters-list>     -> eps
//<function-parameters-list>     -> <parameter-definition> <more-function-parameters>

int function_parameters_list(data_t* sym)
{
	// init
	last_param = &sym->next_param;
	param_count = -1000;
	
	if(getToken() == TOK_RIGHT_PAR)
	{
		ungetToken();
		return SYN_OK;
	} else {
		ungetToken();

		if(parameter_definition() == SYN_ERR)
			return SYN_ERR;
		return more_function_parameters();
	}
}

//<parameter-definition>        -> <type-specifier> simple-identifier

int parameter_definition()
{
	int type;
	type_specifier(&type);
	if(type == VOID) 
		error_and_die(SYNTAX_ERROR,"Expected type-specifier (int,double,String)\n");

	if(getToken() != TOK_ID)
		error_and_die(SYNTAX_ERROR,"Expected a simple-identifier for formal parameter.");

	// in the first pass create all formal parameters = crucial
	if(!isSecondPass)
	{
		if(stable_search_variadic(staticSym, 3, parser_class, parser_fun, getTokString()))
			error_and_die(SEMANTIC_ERROR,"Formal parameter '%s' already declared.", getTokString());
		GEN(">>> Add parameter '%s':'%s'",getTokString(),type2str(type));
		// create a new symbol	
		data_t data;
		fillLocalVarData(&data,type, ++param_count);
		data_t* dt = stable_add_variadic(staticSym, data, 3, parser_class,parser_fun,getTokString());
		// move to next param
		(*last_param) = dt;
		last_param = &dt->next_param;
	}

	return SYN_OK;
}

//<variable-initialization>        -> eps
//<variable-initialization>        -> = <more-next>
int variable_initialization(data_t* variable)
{
	int res = getToken();
	ungetToken();
	if(res == TOK_DELIM)
		return SYN_OK;
	if(getToken() != TOK_ASSIGN)
		error_and_die(SYNTAX_ERROR,"Expected = in inicialization");
	return more_next(variable);
}

//<local-definition>            -> <type-specifier> simple-identifier <variable-initialization> ;
int local_definition()
{
	data_t* def = NULL;
	int type;	
	if(type_specifier(&type) == SYN_ERR)
		error_and_die(SYNTAX_ERROR,"Expected type-specifier (int,double,String)\n");
	if(type == VOID)
		error_and_die(SYNTAX_ERROR,"Definition of void variable.");

	if(getToken() != TOK_ID)
		error_and_die(SYNTAX_ERROR,"Expected simple-id in local definition");
	char* var = getTokString();
	// generate locale vars in the second run
	if(isSecondPass)
	{
		if(stable_search_variadic(staticSym, 3,parser_class, parser_fun,var))
			error_and_die(SEMANTIC_ERROR,"Local symbol '%s' ['%s.%s'] redef."
			" already defined.", var, parser_class,parser_fun);	
		
		data_t data;
		fillLocalVarData(&data, type, 1);
		def = stable_add_variadic(staticSym,data,3, parser_class,parser_fun,var);
	}
	
	if(isSecondPass)
	{
		switch(type)
		{
			case INTEGER:
				create_and_add_instruction(insProgram, INST_PUSH_INT,0,0,0);
				break;
			case STRING:
				create_and_add_instruction(insProgram, INST_PUSH_STRING,0,0,0);
				break;
			case DOUBLE:
				create_and_add_instruction(insProgram, INST_PUSH_DOUBLE,0,0,0);
				break;
		}
	}	
	if(variable_initialization(def) == SYN_ERR)
		return SYN_ERR;
	if(getToken() != TOK_DELIM)
		error_and_die(SYNTAX_ERROR,"Expected ; in local definition");

	
	GEN("Local variable '%s' with type: %s",var,type2str(type));
	return SYN_OK;
}

//<function-body>               -> <statement> <function-body>
//<function-body>               -> <local-definition> <function-body>
//<function-body>               -> eps
int function_body()
{
	// on epsilon-case
	if(getToken() == TOK_RIGHT_BRACE)
	{
		ungetToken();
		return SYN_OK;
	}

	// declaration
	if(isTokenTypeSpecifier())
	{
		ungetToken();
		if(local_definition() == SYN_ERR)
			return SYN_ERR;
		return function_body();
	}
	ungetToken();	
	// statement
	if(statement() == SYN_ERR)
		return SYN_ERR;
	return function_body();
}


//<more-definition>             -> ( <function-parameters-list> ) { <function-body> }
//<more-definition>             -> ;
//<more-definition>             -> = <expr> ;

int more_definition(data_t* sym)
{
	switch(getToken())
	{
		case TOK_LEFT_PAR:
			if(!isSecondPass)
				fillFunctionData(sym,sym->type);

			if(function_parameters_list(sym) == SYN_ERR)
				return SYN_ERR;
			if(!isSecondPass)
			{
				util_correctParamList(sym);
			}
			if(getToken() != TOK_RIGHT_PAR)
				error_and_die(SYNTAX_ERROR,"Expected ')'");		
				
			if(getToken() != TOK_LEFT_BRACE)
				error_and_die(SYNTAX_ERROR,"Expected '{'");		
			if(isSecondPass)
			{
				GEN("Generate and save function label in instruction list.");
				sym->data.data.instruction = create_and_add_instruction(insProgram, INST_LABEL,0,0,0);
			}
			if(function_body() == SYN_ERR)
				return SYN_ERR;
			if(getToken() != TOK_RIGHT_BRACE)
				error_and_die(SYNTAX_ERROR,"Expected '}'");		
			
			GEN("Verify if RETURN was generated somewhere and clear LOCAL VARS");
			return SYN_OK;
		break;
		case TOK_ASSIGN:
			GEN("Assign value");
			if(expression() == SYN_ERR)
				return SYN_ERR;
			// TODO: checkout expression type
			
			// TODO: assign const value to symbol table
			if(isSecondPass)
				create_and_add_instruction(insInit,INST_STORE,&sym->data,0,0);

			if(getToken() != TOK_DELIM)
				error_and_die(SYNTAX_ERROR,"Missing ';' in definition");
		case TOK_DELIM:
			if(!isSecondPass)
			{
				inicializeData(sym);	
				if(sym->type == VOID)
					error_and_die(SYNTAX_ERROR,"Void variable definition");
			}
			return SYN_OK;
		default:
			error_and_die(SYNTAX_ERROR,"Expected ';','=' or '(' in static definition.");
	}
	return SYN_ERR;
	
}




//<definition>                  -> static <type-specifier> simple-identifier <more-definition>

int definition()
{
	getToken();
	if(!isTokenKeyword(KW_STATIC))
		error_and_die(SYNTAX_ERROR,"Expected keyword 'static'");
	int type;	
	if(type_specifier(&type) == SYN_ERR)
		error_and_die(SYNTAX_ERROR,"Expected type-specifier (int,double,String) or void\n");

	if(getToken() != TOK_ID)
		error_and_die(SYNTAX_ERROR,"Expected simple-id");

	data_t* pData = NULL;
	if(!isSecondPass)
	{
		if(stable_search_variadic(staticSym, 2,parser_class,getTokString()))
			error_and_die(SEMANTIC_ERROR,"Static symbol %s.%s already declared",
				parser_class, getTokString());
		GEN("Create a new static symbol '%s' and set its type to '%s'",getTokString(),type2str(type));
		data_t data;
		fillStaticVarData(&data,type);
		pData = stable_add_variadic(staticSym,data,2,parser_class, getTokString());
	} else {
		pData = stable_search_variadic(staticSym,2,parser_class,getTokString());		
	}
	parser_fun = getTokString();
	return more_definition(pData);
}

//<class-body>                  -> epsilon
//<class-body>                  -> <definition> <class-body> 
int class_body()
{
	if(getToken() == TOK_RIGHT_BRACE)
	{
		ungetToken();
		return SYN_OK;
	}
	ungetToken();
	if(definition() == SYN_ERR || class_body() == SYN_ERR)
		return SYN_ERR;

	return SYN_OK;
}


//<class-definition>            -> class <identifier> { <class-body> } 

int class_definition()
{
	getToken();
	if(!isTokenKeyword(KW_CLASS))
		error_and_die(SYNTAX_ERROR,"Expected keyword 'class'\n");	

	if(getToken() != TOK_ID)
		error_and_die(SYNTAX_ERROR,"Expected simple-identifier\n");

	// if it's the first pass
	if(!isSecondPass)
	{
		GEN("Verify if class '%s' was not declared before. If not, add a new symbol into table", getTokString());
		if(stable_get_var(staticSym, getTokString()) != NULL)
		{
			// semantic error, class redefinition	
			error_and_die(SEMANTIC_ERROR,"Class redefinition");
		} else {
			data_t data;
			fillClassData(&data);
			stable_add_var(staticSym, getTokString(),data);
		}
	}
	parser_class = getTokString();

	if(getToken() != TOK_LEFT_BRACE)
		error_and_die(SYNTAX_ERROR,"Expected { in class definition\n");
	if(class_body() == SYN_ERR)
		return SYN_ERR;
	if(getToken() != TOK_RIGHT_BRACE)
		error_and_die(SYNTAX_ERROR,"Expected } in class definition\n");
	return SYN_OK;
}

//<class-definition-list>       -> <class-definition> <class-definition-list>
//<class-definition-list>       -> eps
int class_definition_list()
{
	int result = getToken();
	if(result == TOK_EOF || result == TOK_ERROR)
	{
		ungetToken();
		return SYN_OK;
	}
	ungetToken();

	if(class_definition() == SYN_ERR)
		return SYN_ERR;
	return class_definition_list();
}



//<source-program>              -> <class-definition-list>
int source_program()
{
	int res = class_definition_list();
	// after first pass, turn into second pass
	isSecondPass = 1;
	return res;
}


int main(int argc, char ** argv)
{
	// inicialize & fill default data
	insProgram = init_inst_list();
	insInit = init_inst_list();
	staticSym = stable_init(1024);
	addBuiltInToTable(staticSym);

	// if no INPUT file has been specified
	if(argc < 2)
	{
		error_and_die(INTERNAL_ERROR,"USAGE: filename");
	}
	if(scanner_openFile(argv[1]))
	{
		// first pass of syntactic analyzer
		int result = source_program();
		if(result != SYN_ERR)
		{
			scanner_rewind();
			// second pass
			GEN("--------------- SECOND PASS ---------------");
			result = source_program();
		}
		if(!stable_search_variadic(staticSym,1, "Main.run"))
			error_and_die(SEMANTIC_ERROR, "Missing 'Main.run'");
		stable_print(staticSym);
		stable_destroy(&staticSym);
		
		inst_list_print(insProgram);

		scanner_closeFile();
		fprintf(stderr,"Result: %d\n",result);
		return 0;
	}
	error_and_die(INTERNAL_ERROR,"Failed to open %s",argv[1]);
	return 1;
}



