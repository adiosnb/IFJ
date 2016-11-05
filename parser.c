#include "scanner.h"
#define SYN_OK 1
#define SYN_ERR 2

/******************************************************************************
 * Utility macro definition
 *****************************************************************************/
// Compares the type and integral data of token, DOESN'T GET A NEW TOKEN
#define isTypeOf(tp, of) (g_lastToken.type == tp && g_lastToken.data.integer == of)

// GETS A NEW TOKEN and compares
#define getType(type) (getToken() == type)

// Compares the type of current token with tp
#define isType(tp) (g_lastToken.type == tp)

int state = 1;
// throw error message on error event
#define throw(message) (state)?(state = 0, fprintf(stderr,"[%s:%d] %s \n",__FILE__,\
						__LINE__,message),SYN_ERR):(SYN_ERR) 

// is token a data type (int/double/String) ?
#define isDataType() 	(g_lastToken.data.integer == KW_STRING || \
			g_lastToken.data.integer == KW_INT || \
			g_lastToken.data.integer == KW_VOID|| \
			g_lastToken.data.integer == KW_DOUBLE)

// is the current token a keyword 'kw' ? 
#define isKeyword(kw) (g_lastToken.type == TOK_KEYWORD && g_lastToken.data.integer == kw)
 
// DEBUGGING compiler hint: what has been processed
#define hint(format,...) fprintf(stderr,"[Hint]: "format"\n", ##__VA_ARGS__)

// Is token a term 
#define isTerm() (g_lastToken.type == TOK_ID|| \
		g_lastToken.type == TOK_SPECIAL_ID ||\
		g_lastToken.type == TOK_LITERAL ||\
		g_lastToken.type == TOK_CONST ||\
		g_lastToken.type == TOK_DOUBLECONST)
// ---------------------------------------------------------------------------
// Examples
// ---------------------------------------------------------------------------

// getType(TOK_ID) will call scanner to get another token and compare the type

// isType(TOK_ID) won't call scanner, but compare the current token instead

// return throw("Message") will print out an error message and return SYN_ERR
 
// hint("Message",...); prints out an error message and is used for debugging

/******************************************************************************
 * Functions
 *****************************************************************************/
int statement();

// TODO: replace this function with a properly-coded bottom-up parser
int expr()
{
	//NOTE: this is just a DEMO
	while(getToken() != TOK_DELIM && !isType(TOK_RIGHT_PAR));
	return SYN_OK;
}

// compound-> { ST ST ... }
int comp()
{
	if(!isType(TOK_LEFT_BRACE))
		return throw("Expected a compound statement. Missing {");
	 
	// TODO: make this predictive
	getToken();
	while(!isType(TOK_RIGHT_BRACE))
	{
		if(statement() == SYN_ERR)
			return SYN_ERR;
		getToken();
	}
	return SYN_OK;
}

//<jump-statement> -> return <expr> ;
int jump_statement()
{
	if(expr() == SYN_ERR)
		return SYN_ERR;
	if(!isType(TOK_DELIM))
		return throw("Expected ;");
	return SYN_OK;
}

// <iteration-statement> -> while ( EXPR ) <compound-statement>
int iteration_statement()
{
	
	if(!getType(TOK_LEFT_PAR))
		return throw("Expecting ( after while");
	
	if(expr() == SYN_ERR)
		return SYN_ERR;
	
	if(!isType(TOK_RIGHT_PAR))
		return throw("Expecting ) after while");
	getToken();
	return comp();
}

// <selection-statement> -> if(EXPR) <compound-statement>  else <compound-statement> 
int selection_statement()
{

	if(!getType(TOK_LEFT_PAR))
		return throw("Expecting ( after if");
	
	if(expr() == SYN_ERR)
		return SYN_ERR;
	
	if(!isType(TOK_RIGHT_PAR))
		return throw("Expecting ) after expression: if (EXPR)");

	getToken();
	if(comp() == SYN_ERR)
		return SYN_ERR;
	
	getToken();
	if(!isTypeOf(TOK_KEYWORD,KW_ELSE))
		return throw("Expecting 'else'");
	getToken();
	return comp();
}

// <assign-statement> -> identifier <next>
int assign_statement()
{
	char* ID = g_lastToken.data.string;
	switch(getToken())
	{
		// id = EXPR;
		case TOK_ASSIGN:
			if(expr() == SYN_ERR)
				return SYN_ERR;
			if(isType(TOK_DELIM))
				return SYN_OK;
			return throw("Missing ;");
		// id ( ) 
		case TOK_LEFT_PAR:
			do {
				getToken();
				// expects a term
				if(!isTerm())
					break;
			} while(getType(TOK_LIST_DELIM));

			if(!isType(TOK_RIGHT_PAR))
				return throw("Expected ) or a list of terms");	
			if(!getType(TOK_DELIM))
				return throw("Expected ;");	
			return SYN_OK;
	}	
}
// statement->return EXPR ;
// statement->while(EXPR) COMPOUND 
// statement->if(EXPR) COMPOUND else COMPOUND;
// statement->id = EXPR;
// statement->id ( ) ; 
int statement()
{
	// if the rule starts with keyword (while, if, return)
	//  SELECTION | JUMP | ITERATION 
	if(isType(TOK_KEYWORD))
	{
		switch(g_lastToken.data.integer)
		{
			case KW_RETURN:
				return jump_statement();
			case KW_WHILE:
				return iteration_statement();
			case KW_IF:
				return selection_statement();
			default:
				fprintf(stderr,"Type: %d\n",g_lastToken.data);
				
		}
		return throw("Unexpected keyword ");
	}
	// TODO: statement->id = id();
	// statement->id = EXPR;
	// statement->id ( ) ; 
	else if(isType(TOK_ID) || isType(TOK_SPECIAL_ID))
	{
		return assign_statement();	
	}
	// statement->COMPOUND
	 else if(isType(TOK_LEFT_BRACE))
		return comp();

	return throw("Expecting a valid statement");

}


// function compound
// func-comp->eps
// func-comp->ST func-compound
// func-comp->VAR-DEC func-compound
int funccomp()
{
	if(getType(TOK_RIGHT_BRACE))
		return SYN_OK;	

	if((isType(TOK_KEYWORD) && (isDataType() || g_lastToken.data.integer == KW_STATIC)))
	{
		// TODO: no static definition in function, local variables can only be "local" ?
		// VAR-DEC->static|EPS type id;
		// VAR-DEC->static|EPS type id = EXPR;
		// VAR-DEC block 
			char* funcDec = NULL;
			//TODO: remove if declaration of static var is forbidden in function
			//if(g_lastToken.data.integer == KW_STATIC)
			//	getToken();
			if(!(isType(TOK_KEYWORD) && isDataType()))
				return throw("Expected type-spec");	
			if(!getType(TOK_ID))
				return throw("Expected identifier");	
			funcDec = g_lastToken.data.string;
			if(!getType(TOK_DELIM))
				return throw("Expected ;");	

			hint("Local variable '%s' declared.",funcDec);
			return funccomp();	
	} else
	{
		//statement
		if(statement() == SYN_ERR)
			return SYN_ERR;
		return funccomp();
	}

	return throw("Expected statement or variable declaration in the body of function");
	return SYN_ERR;
}
//cbody->DECL cbody
//cbody->eps
//DECL->static type ID;
//DECL->static type ID = EXPR;
//DECL->static type ID (type id, type id, ...) {FUNCCOMP }
int body()
{
	// choose cbody->eps on }
	if(getToken() == TOK_RIGHT_BRACE)	
		return SYN_OK;
	// otherwise, process cbody->decl
	// decl->static type id; | static type id = EXPR ;
	if(isType(TOK_EOF))
		return throw("Missing '}'");
	if(!isTypeOf(TOK_KEYWORD,KW_STATIC))
		return throw("Awaited 'static'");

	// if we didn't get a proper type such as 'int', 'double', 'etc'
	if(!(getToken() == TOK_KEYWORD && (isDataType())))
		return throw("Awaited 'String','double' or 'int'");
	
	// get identifier
	if(getToken() != TOK_ID)
		return throw("Awaiting simple 'identifier'");

	char* decId = g_lastToken.data.string;
	switch(getToken())
	{
		// static type id;
		case TOK_DELIM:
			// yet another variable
			hint("Variable '%s' declared.",decId);
			break;
		// static type id = EXPR ;
		case TOK_ASSIGN:
			// parse expresion
			if(expr() == SYN_ERR)
				return SYN_ERR;
			if(!isType(TOK_DELIM)) 
				return throw("Expected ';'");
			hint("Variable '%s' declared.",decId);
			break;

		// static type id (type id, type id, ...) { FUNC-COMP }
		case TOK_LEFT_PAR:
			if(!getType(TOK_RIGHT_PAR))
			{
				// TODO: this may not be predictive
				do {
					if(!isType(TOK_KEYWORD))
						return throw("Awaited 'type'");
					if(!isDataType())
						return throw("Expected 'String', 'int' or 'double'");

					if(getToken() != TOK_ID)
						return throw("Expected 'id' after 'type'");
								
					if(getType(TOK_LIST_DELIM))
					{
						getToken();
						continue;
					}	
				} while(!isType(TOK_RIGHT_PAR));
			}
			if(getToken() != TOK_LEFT_BRACE)
				return throw("Expected '{'");	
			// parse the body of function 
			if(funccomp() == SYN_ERR)
				return SYN_ERR;
			if(!isType(TOK_RIGHT_BRACE))
				return throw("Expected '}'");	
			hint("Function '%s' declared.",decId);
			break;

		default:
			return throw("Expected ';','=' or '(' in declaration");
			
	}	

	return body();
		
}

// <class-definition> -> class <id> <compound>
int classdef() {
	// on EOF apply start->eps

	/* class-decl start */
	// class-decl->class id { C-BODY } 
	if(!isTypeOf(TOK_KEYWORD,KW_CLASS))
		return throw("Expected 'class'");

	if(getToken() != TOK_ID)
		return throw("Expected 'indentifier'");	

	char* className = g_lastToken.data.string;
	
	if(!getType(TOK_LEFT_BRACE))
		return throw("Expected '{'");	
	
	// simulate the body of class
	// C-BODY
	if(body() == SYN_ERR)
		return SYN_ERR;	

	if(!isType(TOK_RIGHT_BRACE))
		return throw("Expected '}'");	

	/* class-decl end*/

	// class has been parsed correctly	
	hint("Class '%s' parsed.",className);
}

// <class-definition-list> -> <class-definition> <class-definition-list>
// <class-definition-list> -> eps
int classdeflist()
{
	// choose <class-definition-list> -> eps on token eps
	if(getToken() == TOK_EOF)
		return SYN_OK;
	
	// else continue with: 
	// <class-definition-list> -> <class-definition> <class-definition-list>
	if(classdef() == SYN_ERR)
		return SYN_ERR;

	return classdeflist();
}

int parse()
{
	return classdeflist();
}

int main(int argc, char* argv[])
{
	char name[256] = "source.java";
	if(argc >= 2)
		snprintf(name,255,"%s",argv[1]);

	// try to open the file with source code
	int result = scanner_openFile(name);
	// if the source code has been loaded correctly
	if(result)
	{
		// parse the code
		int result = parse();
		printf("State: %d\n",result);
		return result;
	} else {
		printf("Error has occured while opening the file %s.\n",name);	
	}
	return 0;
}
