
#include <stdio.h>

enum returnCodes { ERROR, OK};

// As different classes require different data, tokens carry an union
// allowing all neccessary kinds of data
typedef union	{
	char*	string;		// for ID/keywords
	double	real;		// for constant
	int	integer;	// for constant
	char	op;		// for operator
} t_tokenData;

typedef struct {
	// the lexical type of token (identifier, keyword, etc)	
	unsigned int 	type;
	t_tokenData	data;	
} t_token;



extern t_token	g_lastToken;

/*---------------------------------------------------------------------------*/
//	Scanner enums	
/*---------------------------------------------------------------------------*/
// Types of TOK_KEYWORD
enum keywords {
	KW_BOOL,	// boolean
	KW_BREAK,	// break
	KW_CLASS,	// class
	KW_CONTINUE,	// continue
	KW_DO,		// do
	KW_DOUBLE,	// double
	KW_ELSE,	// else
	KW_FALSE,	// false
	KW_FOR,		// for
	KW_IF,		// if
	KW_INT,		// int
	KW_RETURN,	// return
	KW_STRING,	// String
	KW_STATIC,	// static
	KW_TRUE,	// true
	KW_VOID,	// void
	KW_WHILE,	// while
};

// Types of lexemes (token lexical classes)
enum	tokenTypes {
	// ==   !=    <    >   <=   >=    +    -    *    /   ID    (    )
	// start of expr tokens
	TOK_EQ,			// ==
	TOK_NOTEQ,		// !=
	TOK_LESS,		// <
	TOK_GREATER,		// >
	TOK_LE,			// <=
	TOK_GE,			// >=
	TOK_PLUS,		// +
	TOK_MINUS,		// -
	TOK_MUL,		// *
	TOK_DIV,		// /
	TOK_ID,			// identifier
	TOK_LEFT_PAR,		// (
	TOK_RIGHT_PAR,		// )
	// end of expr tokens
	TOK_SPECIAL_ID,		// full identifier
	TOK_KEYWORD,		// reserved keyword
	//TOK_OPERATOR,		// operator (.+- etc) //TODO remove this kind of token
	TOK_LITERAL,		// text constant
	TOK_CONST,		// integer constant
	TOK_DOUBLECONST, 	// real constant
	TOK_DELIM,		// ;
	TOK_LIST_DELIM,		// ,
	TOK_LEFT_BRACE,		// {
	TOK_RIGHT_BRACE,	// } 
	TOK_ASSIGN,		// =
	TOK_EOF,
	TOK_ERROR
	};		// end-of-file


/*---------------------------------------------------------------------------*/
//	List of public functions provided by scanner 	
/*---------------------------------------------------------------------------*/

int	scanner_openFile(char* fileName);

int	scanner_closeFile();


int	getToken();
