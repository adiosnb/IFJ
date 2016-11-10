#include <stdio.h>
#include "scanner.h"

enum syntaxCorrectness {SYN_OK,SYN_ERR};

int	isTokenKeyword(int kw)
{
	return (g_lastToken.type == TOK_KEYWORD && g_lastToken.data.integer == kw);
}

int	isIdentifier()
{
	return (g_lastToken.type == TOK_KEYWORD
		||  g_lastToken.type == TOK_SPECIAL_ID);
}


int 	throw(char* error)
{
	static int hasThrown = 0;
	if(!hasThrown)
		fprintf(stderr,error);	
	hasThrown = 1;
	return SYN_ERR;
}

int	hint(char* msg)
{
	fprintf(stderr,msg);
	return 0;
}


int class_definition_list();
int type_specifier();
int iteration_statement();
int jump_statement();
int definition();
int block_items_list();
int compound_statement();
int function_parameters_list();

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
		return SYN_OK;
	} else {
		ungetToken();
		ungetToken();
		return expression();
	}	
}


//<next>                         -> = <more-next>
//<next>                         -> ( <function-parameters-list> ) 

int next()
{	
	switch(getToken())
	{
		case TOK_EQ:
			return more_next();
		case TOK_LEFT_BRACE:
			if(function_parameters_list() == SYN_ERR)
				return SYN_ERR;	
			if(getToken() != TOK_RIGHT_PAR)
				return throw("Expected )");
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
}

//<assign-statement>             -> identifier <next> ;
int assign_statement()
{
	int type = getToken();
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

}


//<block-item>                   -> <definition>
int block_item()
{
	return definition();
}


//<more-block-items>             -> eps
//<more-block-items>             -> <block-items-list>
int more_block_items()
{
	if(getToken() == TOK_RIGHT_BRACE)
	{
		ungetToken();
		return SYN_OK;
	} else {
		ungetToken();
		return block_items_list();	
	}
}

//<block-items-list>             -> <block-item> <more-block-items>
int block_items_list()
{
	if(block_item() == SYN_ERR)	
		return SYN_ERR;

	return more_block_items();
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
	if(isTokenKeyword(KW_STRING))
		return 1;
	else
		ungetToken();
	return 0;
}


//<more-function-parameters>     -> eps
//<more-function-parameters>     -> , <function-parameters-list> 

int more_function_parameters()
{
	switch(getToken())
	{
		case TOK_LIST_DELIM:
				return function_parameters_list();
		default:
			hint("more_function_parameters is strange");
		case TOK_RIGHT_PAR:
			ungetToken();
			return SYN_OK;
	}
	return SYN_ERR;
			
}

//<function-parameters-list>     -> eps
//<function-parameters-list>     -> <definition> <more-function-parameters>

int function_parameters_list()
{
	if(getToken() == TOK_RIGHT_PAR)
	{
		ungetToken();
	} else {
		ungetToken();

		if(definition() == SYN_ERR)
			return SYN_ERR;
		return more_function_parameters();
	}
}

//<variable-initialization>        -> eps
//<variable-initialization>        -> = expression

int variable_initialization()
{
	if(getToken() == TOK_EQ)
	{
		return expression();
		
	} else {
		ungetToken();
		return SYN_OK;
	}
}


//<more-definition>             -> <variable-initialization> ;
//<more-definition>             -> ( <function-parameters-list> ) <compound-statement>
// TODO: id(param,param) 

int more_definition()
{
	if(getToken() == TOK_LEFT_PAR)
	{
		if(function_parameters_list() == SYN_ERR)
			return SYN_ERR;
		if(getToken() != TOK_RIGHT_PAR)
			return throw("Expected ')'");		
			
		return compound_statement();
	} else {
		ungetToken();
		if(variable_initialization() == SYN_ERR)
			return SYN_ERR;
		
		if(getToken() == TOK_DELIM)
			return SYN_ERR;

		return SYN_OK;
	
	}
	
}


//<definition>                  -> <static-type> <type-specifier> <identifier> <more-definition>

int definition()
{
	int isStatic = static_type();
	
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


//<more-class-definition>       -> eps
//<more-class-definition>       -> <class-definition-list> 
int more_class_definition()
{
	if(getToken() == TOK_EOF)
		return SYN_OK;

	ungetToken();
	return class_definition_list();
	
}

//<class-definition-list>       -> <class-definition> <more-class-definition>
int class_definition_list()
{
	class_definition();
	return more_class_definition();
}



//<source-program>              -> <class-definition-list>
int source_program()
{
	return class_definition_list();
}


int main()
{
}



