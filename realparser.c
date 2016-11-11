#include <stdio.h>
#include "scanner.h"

int inFunction = 0;
#define setInFunction(st) inFunction = st

enum syntaxCorrectness {SYN_OK,SYN_ERR};

int	isTokenKeyword(int kw)
{
	return (g_lastToken.type == TOK_KEYWORD && g_lastToken.data.integer == kw);
}

int	isIdentifier()
{
	return (g_lastToken.type == TOK_ID
		||  g_lastToken.type == TOK_SPECIAL_ID);
}

int	isTokenTypeSpecifier()
{
	return (g_lastToken.type == TOK_KEYWORD && (
		g_lastToken.data.integer == KW_VOID ||
		g_lastToken.data.integer == KW_INT ||
		g_lastToken.data.integer == KW_DOUBLE||
		g_lastToken.data.integer == KW_STRING));
}


/*int 	throw(char* error)
{
	static int hasThrown = 0;
	if(!hasThrown)
		fprintf(stderr,error);	
	hasThrown = 1;
	return SYN_ERR;
}
*/

#define hint(format,...) fprintf(stderr,"[Hint]: "format"\n", ##__VA_ARGS__)

#define throw(format,...) fprintf(stderr,"[%s:%d]: "format"\n", __FILE__,__LINE__,##__VA_ARGS__),SYN_ERR



int class_definition_list();
int type_specifier();
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

/*<type-specifier>               -> eps
<type-specifier>               -> String
<type-specifier>               -> int
<type-specifier>               -> double
<type-specifier>               -> void
*/
int type_specifier()
{
	getToken();
	switch(g_lastToken.data.integer)
	{
		case KW_INT:
		case KW_DOUBLE:
		case KW_VOID:
		case KW_STRING:
			return SYN_OK;	
		default:
			ungetToken();
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
	return SYN_OK;
}
//<more-next>                    -> expression 
//<more-next>                    -> <identifier> ( <function-parameters-list> ) 
int more_next()
{
	getToken();
	if(getToken() == TOK_LEFT_PAR)
	{	
		if(function_parameters_list() == SYN_ERR)
			return SYN_ERR;

		if(getToken() != TOK_RIGHT_PAR)
			return throw("Expected )");
	} else {
		ungetToken();
		ungetToken();
		if(expression() == SYN_ERR)
			return SYN_ERR;
	}	
	// both ends on ;
	if(getToken() != TOK_DELIM)
		return throw("Expected ;");
	return SYN_OK;
}


//<next>                         -> = <more-next>
//<next>                         -> ( <function-parameters-list> ) 

int next()
{	
	switch(getToken())
	{
		case TOK_ASSIGN:
			return more_next();
		case TOK_LEFT_PAR:
			if(function_parameters_list() == SYN_ERR)
				return SYN_ERR;	
			if(getToken() != TOK_RIGHT_PAR)
				return throw("Expected )");
			if(getToken() != TOK_DELIM)
				return throw("Expected ;");
			return SYN_OK;
	}
	return throw("Expected = or (");
}


//<jump-statement>               -> return <expr> ;
int jump_statement()
{
	getToken();
	if(!isTokenKeyword(KW_RETURN))
		return throw("Expected return");
	return expression();
}

//<iteration-statement>          -> while ( expression ) <compound-statement>

int iteration_statement()
{
	getToken();
	if(!isTokenKeyword(KW_WHILE))
		return throw("Expected return");

	if(getToken() != TOK_LEFT_PAR)
		return throw("Expected (");

	if(expression() == SYN_ERR)
		return SYN_ERR;

	if(getToken() != TOK_RIGHT_PAR)
		return throw("Expected )");

	return compound_statement();
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

		if(getToken() != TOK_RIGHT_PAR)
			return throw("Expected )");
	
		
		if(compound_statement() == SYN_ERR)
			return SYN_ERR;		

		getToken();

		if(!isTokenKeyword(KW_ELSE))
			return throw("Expected else");

		if(compound_statement() == SYN_ERR)
			return SYN_ERR;		
		
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
		return statement();
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
	ungetToken();
	switch(type)
	{
		case TOK_KEYWORD:
		{
			if(isTokenKeyword(KW_RETURN))
				return jump_statement();
			else if(isTokenKeyword(KW_IF))
				return selection_statement();
			else if(isTokenKeyword(KW_WHILE))
				return iteration_statement();
			else
				return throw("Got keyword, Expected if, while or return");
			break;
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
//<variable-initialization>        -> = expression

int variable_initialization()
{
	if(getToken() == TOK_ASSIGN)
	{
		return expression();
		
	} else {
		ungetToken();
		return SYN_OK;
	}
}


//<more-definition>             -> <variable-initialization> ;
//<more-definition>             -> ( <function-parameters-list> ) <compound-statement>

int more_definition()
{
	if(getToken() == TOK_LEFT_PAR)
	{
		if(!inFunction)
			setInFunction(1);
		else
			return throw("Err - declaration of function in function");

		if(function_parameters_list() == SYN_ERR)
			return SYN_ERR;
		if(getToken() != TOK_RIGHT_PAR)
			return throw("Expected ')'");		
			
		if(compound_statement() == SYN_ERR)
			return SYN_ERR;
		
		setInFunction(0);
		return SYN_OK;
	} else {
		ungetToken();
		if(variable_initialization() == SYN_ERR)
			return SYN_ERR;
		
		if(getToken() != TOK_DELIM)
			return throw("Missing ';'");

		return SYN_OK;
	
	}
	
}
//<parameter-definition>                  -> <type-specifier-opt> <identifier> 

int parameter_definition()
{
	if(type_specifier() == SYN_ERR)
		return throw("Expected type-specifier (void,int,double,String)\n");

	// TODO - special ID	
	if(getToken() != TOK_ID)
		return throw("Expected simple-id");

	return SYN_OK;
}



//<definition>                  -> <static-type> <type-specifier> <identifier> <more-definition>

int definition()
{
	int isStatic = static_type();
	if(inFunction && isStatic)
		return throw("Expected non-static definition in function");
	else if(inFunction == 0 && isStatic == 0)
		return throw("Expected static keyword for symbol defined in class");
	hint("IsStatic ? %d",isStatic);
	
	if(type_specifier() == SYN_ERR)
		return throw("Expected type-specifier (void,int,double,String)\n");
	
	if(getToken() != TOK_ID)
		return throw("Expected simple-id");

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
	hint("Result is: %d",result);

	if(class_definition() == SYN_ERR)
		return SYN_ERR;
	return class_definition_list();
}



//<source-program>              -> <class-definition-list>
int source_program()
{
	return class_definition_list();
}


int main()
{
	if(scanner_openFile("source.java"))
	{
		int result = source_program();
		scanner_closeFile();
		fprintf(stderr,"Result: %d\n",result);
		return 0;
	}
	return 1;
}



