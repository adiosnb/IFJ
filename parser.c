#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "scanner.h"

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

enum types {INTEGER, DOUBLE, STRING,FUN_INTEGER,FUN_DOUBLE,FUN_STRING,FUN_VOID,TYPELESS};

char* type2str(int type)
{
	static char* str[] = {"INTEGER","DOUBLE","STRING","FUN_INTEGER","FUN_DOUBLE","FUN_STRING", "FUN_VOID", "TYPELESS"};
	if(type > TYPELESS)
		type = TYPELESS;
	return str[type];
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
			*out_type = FUN_VOID;
			break;
		case KW_STRING:
			*out_type = STRING;
			break;
	}
	return SYN_OK;	
}

//<type-specifier-opt>               ->type-specifier 
//<type-specifier-opt>               ->eps
int type_specifier_opt(int* out_type)
{
	if(type_specifier(out_type) == SYN_ERR)
	{
		ungetToken();
		*out_type = TYPELESS;
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
int more_next()
{
	int isPrint = 0;
	if(getToken() == TOK_SPECIAL_ID)
		isPrint = !strcmp(getTokString(),"ifj16.print");
	if(getToken() == TOK_LEFT_PAR)
	{
		if(inFunction == 0)
			return throw("Initialization of static variable with function call.");	
		GEN("Verify functioncall-variable type compatibility");
		// is it a ID = ifj16.print() case ?
		if(isPrint)
		{
			builtin_print();
			// NOTE: it's a semantic error in any case, just check syntax
			return throw("Semantic error");
		}
		else {
			hint("MORE-Function call of '%s'",getTokString());
			setInCall(1);
			if(function_parameters_list() == SYN_ERR)
				return SYN_ERR;
			setInCall(0);

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

int next()
{	
	int isPrint = !strcmp(getTokString(),"ifj16.print");
	switch(getToken())
	{
		case TOK_ASSIGN:
			if(more_next() == SYN_ERR)
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

				setInCall(1);
				if(function_parameters_list() == SYN_ERR)
					return SYN_ERR;	
				GEN("Generate a function call INSTR");
				setInCall(0);
				if(getToken() != TOK_RIGHT_PAR)
					return throw("Expected )");
				if(getToken() != TOK_DELIM)
					return throw("Expected ;");
				return SYN_OK;
			}
	}
	return throw("Expected = or (");
}


//<jump-statement>               -> return <expr> ;
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
		GEN("Check if variable '%s'  is defined", getTokString());
		return next();
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
		incIdent();
		int res = statement();
		decIdent();
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

//<static-type>                  -> static
//<static-type>                 -> eps

int static_type()
{
	getToken();
	if(isTokenKeyword(KW_STATIC))
		return 1;
	else
		ungetToken();
	return 0;
}


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
		default:
			hint("more_function_parameters is strange");
		case TOK_RIGHT_PAR:
			ungetToken();
			return SYN_OK;
	}
	return SYN_ERR;
			
}

//<function-parameters-list>     -> eps
//<function-parameters-list>     -> <parameter-definition> <more-function-parameters>

int function_parameters_list()
{
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

//<variable-initialization>        -> eps
//<variable-initialization>        -> = <more-next>

int variable_initialization()
{
	int res = getToken();
	ungetToken();
	if(res == TOK_DELIM)
		return SYN_OK;
	if(getToken() != TOK_ASSIGN)
		return throw("Expected = in inicialization");
	return more_next();
}


//<more-definition>             -> <variable-initialization> ;
//<more-definition>             -> ( <function-parameters-list> ) <compound-statement>

int more_definition()
{
	if(getToken() == TOK_LEFT_PAR)
	{
		if(inFunction)
			return throw("Declaration of function in function");

		setInFunction(1);
		
		setInCall(0);
		if(function_parameters_list() == SYN_ERR)
			return SYN_ERR;
		if(getToken() != TOK_RIGHT_PAR)
			return throw("Expected ')'");		
			
		GEN("Generate and save function label in instruction list.");
		if(compound_statement() == SYN_ERR)
			return SYN_ERR;
		
		setInFunction(0);
		GEN("Verify if RETURN was generated somewhere and clear LOCAL VARS");
		return SYN_OK;
	} else {
		// TODO: semantic: check if variable is not declared with void
		ungetToken();
		if(variable_initialization() == SYN_ERR)
			return SYN_ERR;
		
		if(getToken() != TOK_DELIM)
			return throw("Missing ';' in definition");

		return SYN_OK;
	
	}
	
}
//<parameter-definition>                  -> <type-specifier-opt> <identifier> 

int parameter_definition()
{
	hint("Parameter definition - isCall: %d",inCall);
	// if we define a formal parameter, we require a type specifier
	int type;
	type_specifier_opt(&type);
	if(inCall == 0 && type == TYPELESS || type == FUN_VOID) 
		return throw("Expected type-specifier (void,int,double,String)\n");
	
	// if we are parsing a function call e.g. add(10,20);
	if(inCall)
	{
		switch(getToken())
		{
			case TOK_DOUBLECONST:
			case TOK_CONST:
			case TOK_LITERAL:
			case TOK_SPECIAL_ID:
			case TOK_ID:
				GEN("Compare the type of argument with formal param and generate a PUSH.");
				break;
			default:
					return throw("Expected any identifier/constant");
				
			
		}
	} else {
		// otherwise we are parsing a function definition
		if(getToken() != TOK_ID)
			return throw("Expected simple indentifier");
		GEN("Add a new formal parameter '%s' of type '%s' into function's list of params",getTokString(), type2str(type));
	}

	return SYN_OK;
}



//<definition>                  -> <static-type> <type-specifier> <identifier> <more-definition>

int definition()
{
	// is declaration if { } of some statement
	if(ident != 0)
		return throw("Declaration in blocks are forbidden.");

	// if not, continue a determine if declaration is static
	int isStatic = static_type();
	if(inFunction && isStatic)
		return throw("Expected non-static definition in function");
	else if(inFunction == 0 && isStatic == 0)
		return throw("Expected static keyword for symbol defined in class");

	int type;	
	if(type_specifier(&type) == SYN_ERR)
		return throw("Expected type-specifier (void,int,double,String)\n");

	
	if(getToken() != TOK_ID)
		return throw("Expected simple-id");

	if(inFunction == 0)
		GEN("Create a new static symbol '%s' and set its type to '%s'",getTokString(),type2str(type));
	else
		GEN("Create a new local symbol '%s' and set its type to '%s'",getTokString(),type2str(type));

	return more_definition();
}


//<class-definition>            -> class <identifier> <compound-statement>

int class_definition()
{
	getToken();
	if(!isTokenKeyword(KW_CLASS))
		return throw("Expected keyword 'class'\n");	

	if(getToken() != TOK_ID)
		return throw("Expected identifier\n");

	GEN("Verify if class '%s' was not declared before. If not, add a new symbol into table", getTokString());

	return compound_statement();


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
	return class_definition_list();
}


int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		return throw("USAGE: filename");
	}
	if(scanner_openFile(argv[1]))
	{
		int result = source_program();
		scanner_closeFile();
		fprintf(stderr,"Result: %d\n",result);
		return 0;
	}
	return throw("Failed to open %s",argv[1]);
	return 1;
}



