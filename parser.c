//#define	DEBUG 1 
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "stable.h"
#include "error.h"


stab_t	*staticSym = NULL;

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
}
void fillStaticVarData(data_t* data,int type)
{
	data->type = type;
	data->data.arg_type = type;	
	data->data.data.i = UNINITIALIZED;
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

void addBuiltInToTable(stab_t* table)
{
	data_t data, *ptr;
	// class IFJ16
	fillClassData(&data);
	stable_add_var(table, "ifj16",data);
	// print 
	fillFunctionData(&data,VOID);
	stable_add_variadic(table,data,2,"ifj16","print");
	// readInt 
	fillFunctionData(&data,INTEGER);
	stable_add_variadic(table,data,2,"ifj16","readInt");
	// readDouble
	fillFunctionData(&data,DOUBLE);
	stable_add_variadic(table,data,2,"ifj16","readDouble");
	// readString
	fillFunctionData(&data,STRING);
	stable_add_variadic(table,data,2,"ifj16","readString");
	 
	// length 
	fillLocalVarData(&data,INTEGER, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","length","s");

	fillFunctionData(&data,INTEGER);
	data.next_param = ptr;
	stable_add_variadic(table,data,2,"ifj16","length");

	// TODO: substr, compare, find
	// substr
	fillLocalVarData(&data,INTEGER, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","substr","n");
	fillLocalVarData(&data,INTEGER, -3);
	ptr =  stable_add_variadic(table,data,3,"ifj16","substr","i");
	fillLocalVarData(&data,STRING, -4);
	ptr =  stable_add_variadic(table,data,3,"ifj16","substr","s");
	fillFunctionData(&data,STRING);
	data.next_param = ptr;
	stable_add_variadic(table,data,2,"ifj16","substr");

	//compare 
	fillLocalVarData(&data,STRING, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","compare","s2");
	fillLocalVarData(&data,STRING, -3);
	ptr =  stable_add_variadic(table,data,3,"ifj16","compare","s1");
	fillFunctionData(&data,INTEGER);
	data.next_param = ptr;
	stable_add_variadic(table,data,2,"ifj16","compare");
	// find
	fillLocalVarData(&data,STRING, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","find","s");
	fillLocalVarData(&data,STRING, -3);
	ptr =  stable_add_variadic(table,data,3,"ifj16","find","search");
	fillFunctionData(&data,INTEGER);
	data.next_param = ptr;
	stable_add_variadic(table,data,2,"ifj16","find");

	//sort 
	fillLocalVarData(&data,STRING, -2);
	ptr =  stable_add_variadic(table,data,3,"ifj16","sort","s");
	fillFunctionData(&data,STRING);
	data.next_param = ptr;
	stable_add_variadic(table,data,2,"ifj16","sort");



}

char* type2str(int type)
{
	static char* str[] = {"INTEGER","DOUBLE","STRING",NULL,NULL,NULL,"VOID"};
	return str[type];
}

int util_corretParamList(data_t* func)
{
	data_t* ptr = func->next_param;
	if(!ptr)
		return 0;
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
			return throw("Semantic error");
		} else 
		{
			data_t*	func = NULL, *data = NULL;
			if(isSecondPass)
			{
				if(getLastToken() == TOK_ID)
					func = stable_search_variadic(staticSym, 2, parser_class, getTokString());
				else
					func = stable_search_variadic(staticSym, 1, getTokString());
				if(!func)
					return throw("SEMANTIC - function '%s' not found.",getTokString());
				if(func->data.arg_type != INSTRUCTION)
					return throw("SEMANTIC - expecting '%s' to be function", getTokString());
				if(func->type != var->type)
					return throw("SEMANTIC - function call type dismatch");
				data = func->next_param;
			}
			getToken();

			if(function_arguments_list(&data) == SYN_ERR)
				return SYN_ERR;

			if(data != NULL)
				return throw("SEMANTIC - too few arguments in funciton call");

			if(getToken() != TOK_RIGHT_PAR)
				return throw("Expected )");
		}
		return SYN_OK;
	} else {
		ungetToken();
		ungetToken();
		if(expression() == SYN_ERR)
			return SYN_ERR;
		GEN("Verify if result of expr can be assigned. If do, generate assign");
	}	
	return SYN_OK;
}
// builtin_print is a special case of function call which supports
// a concatenation as an argument
int builtin_print()
{
	if(getToken() == TOK_RIGHT_PAR)
		return throw("Expected a term or concatenation");
	ungetToken();	
	do {
		int res = getToken();
		switch(res)
		{
			case TOK_ID:
			case TOK_SPECIAL_ID:
			case TOK_CONST:
			case TOK_DOUBLECONST:
			case TOK_LITERAL:
				break;
			default:
				return throw("Expected an identifier or a constant.");
		}
		
		res = getToken();
		if(res == TOK_RIGHT_PAR)
			break;
		if(res != TOK_PLUS)
			return throw("Expected +");

	} while (1);
	if(getToken() != TOK_DELIM)
		return throw("Expected ;");

	return SYN_OK;
	
}


//<next>                         -> = <more-next>
//<next>                         -> ( <function-parameters-list> ) 

int next(data_t* symbol)
{	
	int isPrint = !strcmp(getTokString(),"ifj16.print");
	switch(getToken())
	{
		case TOK_ASSIGN:
			if(more_next(symbol) == SYN_ERR)
				return SYN_ERR;
			if(getToken() != TOK_DELIM)
				return throw("Expected ;");
			return SYN_OK;
		case TOK_LEFT_PAR:
			if(isPrint)
			{
				return builtin_print();
			} else {
				GEN("Check if symbol '%s' is defined", getTokString());

				if(function_arguments_list() == SYN_ERR)
					return SYN_ERR;	
				GEN("Generate a function call INSTR");
				if(getToken() != TOK_RIGHT_PAR)
					return throw("Expected )");
				if(getToken() != TOK_DELIM)
					return throw("Expected ;");
				return SYN_OK;
			}
	}
	return throw("Expected = or (");
}


//<jump-statement>               -> return <expression> ;
int jump_statement()
{
	getToken();
	if(!isTokenKeyword(KW_RETURN))
		return throw("Expected return");
	if(expression() == SYN_ERR)
		return SYN_ERR;
	if(getToken() != TOK_DELIM)
		return throw("Expected ; at the end of return statement.");

	GEN("Generate RETURN instruction and expression");

	return SYN_OK;

}

//<iteration-statement>          -> while ( expression ) <compound-statement>

int iteration_statement()
{
	getToken();
	if(!isTokenKeyword(KW_WHILE))
		return throw("Expected return");

	GEN("Generate WHILE.test label"); 
	if(getToken() != TOK_LEFT_PAR)
		return throw("Expected (");

	if(expression() == SYN_ERR)
		return SYN_ERR;

	if(getToken() != TOK_RIGHT_PAR)
		return throw("Expected )");

	GEN("Generate COMPARE and JUMP test"); 

	if(compound_statement() == SYN_ERR)
		return SYN_ERR;
	GEN("Generate WHILE_SKIP label and altern CMP jmp addres");

	return SYN_OK;
}

//<selection-statement>          -> if ( expression ) <compound-statement> else <compound-statement>
int selection_statement()
{
	getToken();
	if(isTokenKeyword(KW_IF))
	{
		if(getToken() != TOK_LEFT_PAR)
			return throw("Expected (");
		
		if(expression() == SYN_ERR)
			return SYN_ERR;

		GEN("Generate a CMP and JUMP.");

		if(getToken() != TOK_RIGHT_PAR)
			return throw("Expected )");
	
		
		if(compound_statement() == SYN_ERR)
			return SYN_ERR;		

		GEN("Generate a JMP.");
		GEN("Generate a IF_ELSE label and altern JMP");

		getToken();
		if(!isTokenKeyword(KW_ELSE))
			return throw("Expected else");

		if(compound_statement() == SYN_ERR)
			return SYN_ERR;		
		
		GEN("Generate a IF_SKIP label and altern JMP.");
		return SYN_OK;	
		
	}	
	return throw("Expected IF");
}

//<assign-statement>             -> identifier <next> ;
int assign_statement()
{
	getToken();
	if(isIdentifier())
	{	
		data_t* symbol = NULL;
		GEN("Check if variable '%s'  is defined", getTokString());

		if(isSecondPass)
		{
			if(getLastToken() == TOK_ID)
				symbol = stable_search_variadic(staticSym,3,parser_class,parser_fun,getTokString());
			else
				symbol = stable_search_variadic(staticSym,1,getTokString());
			if(!symbol)
				return throw("SEMANTIC error - symbol %s is missin",getTokString());
		}
		return next(symbol);
	} else
		return throw("Expected identifier or full-identifier.");
}
//<compound-statement>           -> { <block-items-list> }
int compound_statement()
{
	if(getToken() != TOK_LEFT_BRACE)
		return throw("Expected {");

	if(block_items_list() == SYN_ERR)
		return SYN_ERR;

	if(getToken() != TOK_RIGHT_BRACE)
		return throw("Expected {");
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
		return throw("Missing }");
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
					return throw("Got keyword, Expected if, while or return");
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
	return throw("Expected a statement");
}

//<argument-definition>	-> <term>
int argument_definition(data_t** fun)
{
	int arg_type = VOID;
	data_t*	var;
	if(isSecondPass)
	{
		if(!(*fun))
			return throw("Semantic error: too many arguments");
		switch(getToken())
		{
			case TOK_ID:
				var = stable_search_variadic(staticSym, 3, parser_class, parser_fun,getTokString());
				if(!var)
					var = stable_search_variadic(staticSym,2, parser_class,getTokString());
				if(!var)
					return throw("SEMANTIC error - '%s' is not a valid symbol.", getTokString());
				arg_type = var->type;
				break;
			case TOK_SPECIAL_ID:
				var = stable_search_variadic(staticSym, 2, parser_class ,getTokString());
				if(!var)
					return throw("SEMANTIC error - '%s' is not a valid symbol.", getTokString());
				arg_type = var->type;
				break;
			case TOK_LITERAL:
				arg_type = STRING;
				break;
			case TOK_CONST:
				arg_type = INTEGER;
				break;
			case TOK_DOUBLECONST:
				arg_type = DOUBLE;
				break;
			default:
				return throw("Expected a term in function call");
		}
		if(arg_type != (*fun)->type)
			return throw("SEM - argument type dismatch.");
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
				return throw("Expected a term in function call");
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
			return throw("Expected ,");
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
			return throw("Expected ,");
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
		return throw("Expected type-specifier (void,int,double,String)\n");

	if(getToken() != TOK_ID)
		return throw("Expected a simple-identifier for formal parameter.");

	if(isSecondPass)
	{
		if(stable_search_variadic(staticSym, 3, parser_class, parser_fun, getTokString()))
			return throw("SEMANTIC - Formal parameter '%s' already declared.", getTokString());
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
		return throw("Expected = in inicialization");
	return more_next(variable);
}

//<local-definition>            -> <type-specifier> simple-identifier <variable-initialization> ;
int local_definition()
{
	data_t* def = NULL;
	int type;	
	if(type_specifier(&type) == SYN_ERR)
		return throw("Expected type-specifier (int,double,String)\n");
	if(type == VOID)
		return throw("SEMANTIC error -> void definition of local symbol.");

	if(getToken() != TOK_ID)
		return throw("Expected simple-id in local definition");
	char* var = getTokString();
	// generate locale vars in the second run
	if(isSecondPass)
	{
		if(stable_search_variadic(staticSym, 3,parser_class, parser_fun,var))
			return throw("SEMANTIC - Local symbol '%s' of function '%s.%s'"
			" already defined.", var, parser_class,parser_fun);	
		
		data_t data;
		fillLocalVarData(&data, type, 1);
		def = stable_add_variadic(staticSym,data,3, parser_class,parser_fun,var);
	}
	
	if(variable_initialization(def) == SYN_ERR)
		return SYN_ERR;
	if(getToken() != TOK_DELIM)
		return throw("Expected ; in local definition");

	
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
			if(isSecondPass)
			{
				util_corretParamList(sym);
			}
			if(getToken() != TOK_RIGHT_PAR)
				return throw("Expected ')'");		
				
			if(getToken() != TOK_LEFT_BRACE)
				return throw("Expected '{'");		
			GEN("Generate and save function label in instruction list.");
			if(function_body() == SYN_ERR)
				return SYN_ERR;
			if(getToken() != TOK_RIGHT_BRACE)
				return throw("Expected '}'");		
			
			GEN("Verify if RETURN was generated somewhere and clear LOCAL VARS");
			return SYN_OK;
		break;
		case TOK_ASSIGN:
			GEN("Assign value");
			if(expression() == SYN_ERR)
				return SYN_ERR;
			
			// TODO: assign const value to symbol table

			if(getToken() != TOK_DELIM)
				return throw("Missing ';' in definition");
		case TOK_DELIM:
			if(!isSecondPass)
			{
				inicializeData(sym);	
				if(sym->type == VOID)
					return throw("SEMANTIC ERROR - Void variable definition");
			}
			return SYN_OK;
		default:
			return throw("Expected ';','=' or '(' in static definition.");
	}
	return SYN_ERR;
	
}




//<definition>                  -> static <type-specifier> simple-identifier <more-definition>

int definition()
{
	getToken();
	if(!isTokenKeyword(KW_STATIC))
		return throw("Expected keyword 'static'");
	int type;	
	if(type_specifier(&type) == SYN_ERR)
		return throw("Expected type-specifier (void,int,double,String)\n");

	if(getToken() != TOK_ID)
		return throw("Expected simple-id");

	data_t* pData = NULL;
	if(!isSecondPass)
	{
		if(stable_search_variadic(staticSym, 2,parser_class,getTokString()))
			return throw("SEMANTIC ERROR - Static symbol %s.%s already declared",
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
		return throw("Expected keyword 'class'\n");	

	if(getToken() != TOK_ID)
		return throw("Expected simple-identifier\n");

	// if it's the first pass
	if(!isSecondPass)
	{
		GEN("Verify if class '%s' was not declared before. If not, add a new symbol into table", getTokString());
		if(stable_get_var(staticSym, getTokString()) != NULL)
		{
			// semantic error, class redefinition	
			return throw("SEMANTIC - class redefinition");
		} else {
			data_t data;
			fillClassData(&data);
			stable_add_var(staticSym, getTokString(),data);
		}
	}
	parser_class = getTokString();

	if(getToken() != TOK_LEFT_BRACE)
		return throw("Expected { in class definition\n");
	if(class_body() == SYN_ERR)
		return SYN_ERR;
	if(getToken() != TOK_RIGHT_BRACE)
		return throw("Expected } in class definition\n");
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
	if(argc < 2)
	{
		return throw("USAGE: filename");
	}
	if(scanner_openFile(argv[1]))
	{
		staticSym = stable_init(1024);
		addBuiltInToTable(staticSym);
		int result = source_program();
		if(result != SYN_ERR)
		{
			scanner_rewind();
			// second pass
			GEN("--------------- SECOND PASS ---------------");
			result = source_program();
		}
		stable_print(staticSym);
		stable_destroy(&staticSym);

		scanner_closeFile();
		fprintf(stderr,"Result: %d\n",result);
		return 0;
	}
	return throw("Failed to open %s",argv[1]);
	return 1;
}



