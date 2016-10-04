
#include <stdio.h>

// Types of lexemes (token lexical classes)
enum	tokenTypes {
	TOK_ID,			// identifier
	TOK_SPECIAL_ID,		// full identifier
	TOK_KEYWORD,		// reserved keyword
	TOK_OPERATOR,		// operator (.+- etc)
	TOK_LITERAL,		// text constant
	TOK_CONST,		// integer constant
	TOK_DOUBLECONST, 	// real constant
	TOK_DELIM,		// ;
	TOK_LEFT_PAR,		// (
	TOK_RIGHT_PAR,		// )
	TOK_LEFT_BRACE,		// {
	TOK_RIGHT_BRACE,	// } 
	TOK_ASSIGN,		// =
	TOK_EQ,			// ==
	TOK_GE,			// >=
	TOK_LE,			// <=
	TOK_NOTEQ,		// !=
	TOK_LESS,		// <
	TOK_GREATER,		// >
	TOK_EOF};		// end-of-file

enum returnCodes { ERROR, OK};

// As different classes require different data, tokens carry a union
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

// TODO: remove and let it just for scanner.c
extern FILE*	fHandle;


int	scanner_openFile(char* fileName);

int	scanner_closeFile();

extern t_token	g_lastToken;

int	getToken();
