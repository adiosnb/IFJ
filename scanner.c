#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "scanner_alt.h"
#include "str.h"

FILE*	fHandle = NULL;
string_t first, second, literal;

/* Single-linked list of tokens, used only by scanner module*/
typedef struct listTokenElement 
{
	t_token	token;
	struct listTokenElement* next;
	struct listTokenElement* prev;
} t_tokenElem;


// single linked list of tokens
t_tokenElem* tokenList = NULL;

// pointer to current token
t_tokenElem*	currentToken = NULL;

// global structure to get the data returned by last getToken 
t_token	g_lastToken;

// Utility func

void freeToken(t_token* tok)
{
	// delete string
	if(tok->type == TOK_ID  
		|| tok->type == TOK_LITERAL
		|| tok->type == TOK_SPECIAL_ID)
		free(tok->data.string);
}

/* Public functions */

int	scanner_openFile(char* fileName)
{
	fHandle = fopen(fileName,"r");	
	if(fHandle)
		return 1;
	return 0;
}

int	scanner_closeFile()
{
	if(fHandle)
	{
		fclose(fHandle);
		fHandle = NULL;
		
		// clean single linked list
		t_tokenElem *ptr = tokenList,*helpptr = tokenList;
		while(ptr)
		{
			helpptr = ptr;
			ptr = ptr->next;
			freeToken(&helpptr->token);
			free(helpptr);
		}
		tokenList = NULL;
	} 
	return 0;
}


int	scanner_rewind()
{
	// set pointer to the start of linked list
	currentToken = NULL;
	return 0;
}

int	ungetToken()
{
	if(currentToken)
		currentToken = currentToken->prev;
	return 0;
}


char*	createString(const char* str)
{
	char* nstr = malloc(strlen(str)+1);
	if(nstr)
	{
		strcpy(nstr,str);
		return nstr;
	}
	return NULL;
}

char*	createString2(const char* str, const char* second)
{
	char* nstr = malloc(strlen(str)+strlen(second)+2);
	if(nstr)
	{
		sprintf(nstr,"%s.%s",str,second);
		return nstr;
	}
	return NULL;
}


// processes text literals such as "text" or "tex\t"
int	process_literal()
{
    str_reinit(&literal);
	fgetc(fHandle);
	// a normal state is used when awaiting regular ASCII input
	// SPECIAL state is reached after receiving '\' 
	enum States {NORMAL, SPECIAL,OCTAL};
	int c, state = NORMAL;

	int octBase= 64;
	char sum = 0;
	while((c = fgetc(fHandle)) != EOF)
	{
		switch(state)
		{
			case NORMAL:
				if(c == '\"')
				{
					// when the end of terminal is reached
					g_lastToken.type = TOK_LITERAL;
					g_lastToken.data.string = literal.str;
					// TODO: add pointer to symbol
					return TOK_LITERAL;

				} else if(c == '\\')
				{
					state = SPECIAL;
					break;
				} else {
					ADD_CHAR(literal,c);
				}
				break;
			// process escape sequences (e.g. \n) or report invalid ones
			case SPECIAL:
				switch(c)
				{
					case '0': case '1': case '2': case '3': 
						ungetc(c,fHandle);
						state = OCTAL;
						break;
					case '\\':
					case '\"':
                        ADD_CHAR(literal,c);
						state = NORMAL;
						break;
					case 'n':
                        ADD_CHAR(literal,'\n');
						state = NORMAL;
						break;
					case 't':
                        ADD_CHAR(literal,'\t');
						state = NORMAL;
						break;
					default:

						// TODO: report an error - invalid escape sequence
						fprintf(stderr, "Error while reading literal\n");
						return TOK_ERROR;
						break;
				}
				break;
			// special case for processing \ddd
			case OCTAL:
				if(isdigit(c))
				{
					// convert ASCII char to a number of <0,9>
					int digit = c-'0';
					if(digit > 7)
						return TOK_ERROR;
					// and multiply it with N power of 8
					// => conversing octal-to-decal with one buffer sum 
					sum += octBase*digit;
					octBase /= 8;
					// if we have read 3 numbers
					if(octBase == 0)
					{
						//concatenate a new char
                        ADD_CHAR(literal,sum);
						octBase= 64;
						sum = 0;
						// and continue reading the rest of literal
						state = NORMAL;
					}
				} else {
					return TOK_ERROR;
				}	
				break;
		}
	}
	return TOK_ERROR;
}

// Utility function
// TODO: implement a faster way of searching for strings (tree)
//

// Returns 1 if ID is an keyword (type of keyword is returned in 2nd param)
// Returns 0 otherwise
int	isKeyword(const char* str,int* typeOfKeyword)
{
	static char* keywords[] = {"boolean","break","class","continue","do","double",
	"else","false","for","if","int","return","String","static","true","void","while",NULL};

	for(int i = 0; keywords[i] != NULL; i++)
	{
		if(strcmp(keywords[i],str) == 0)
		{
			(*typeOfKeyword) = i;
			return 1;
		}
	}
	return 0;
}

int	process_identifier()
{
	str_reinit(&first);
	str_reinit(&second);

	int c;
	// nonAlpha is toggled to 1 if at least one character is non-alphanumerical
	// -> useful for skipping keyword comparing 
	int isNonAlpha = 0;

	enum states {FIRST,SECOND};
	int state = FIRST;
	while((c = fgetc(fHandle)) != EOF)
	{
		int res = isalnum(c);
	
		// if character fullfills ID requirements 
		if(res || c == '$' || c == '_')
		{
			isNonAlpha |= (res == 0);
		
			// store the incoming char into either first or second part of ID
			if(state == FIRST) {
				//ADD_CHAR(first, c);
				str_add_char(&first,c);
			}else {
				//ADD_CHAR(second,c);}
				str_add_char(&second, c);
			}
		} else {
			// if it is a ID splitter (FIRST.SECOND)
			if(c == '.')
			{
				// then change the state to second
				if(state == FIRST)
					state = SECOND;
				else {
					// already processing the second part of ID
					// > lex. error
					fprintf(stderr,"Error: Multiple '.' in identifier.\n");
					return TOK_ERROR;
				}
				continue;
			}
			
			// the last character is probably a part of another token->return back
			ungetc(c,fHandle);

			// if we captured only a single word, it migh be a keyword
			if(state != SECOND)
			{
				// verify if string isn't a keyword
				if(!isNonAlpha)
				{
					int typeOfKeyword;
					// if ID is in set of keywords
					if(isKeyword(first.str,&typeOfKeyword))
					{
						g_lastToken.type = TOK_KEYWORD;
						g_lastToken.data.integer = typeOfKeyword; 
						return TOK_KEYWORD;
					}
					
				}
			}
			// otherwise, it's an ID
			g_lastToken.type = TOK_ID;
			
			// if it's a special ID (ID.ID)
			if(state == SECOND)
			{
				// if the second part of ID fullfills requirements
				if((first.len && second.len))
				{
					g_lastToken.data.string = second.str;
					g_lastToken.type = TOK_SPECIAL_ID;
				}
				else {
					// error in the second part of ID
					return TOK_ERROR;
				}
			} else 
				g_lastToken.data.string = first.str;
				return g_lastToken.type;
		}
	}

	if (first.len) {
		if (state == FIRST){
			int typeOfKeyword;
			if(isKeyword(first.str,&typeOfKeyword))
			{
				g_lastToken.type = TOK_KEYWORD;
				g_lastToken.data.integer = typeOfKeyword;
				return TOK_KEYWORD;
			} else {
				g_lastToken.type = TOK_ID;
				g_lastToken.data.string = first.str;
				return TOK_ID;
			}
		} else {
			if (second.len){
				g_lastToken.type = TOK_ID;
				g_lastToken.data.string = second.str;
				return TOK_ID;
			} else {
				return TOK_ERROR;
			}
		}
	}

	return TOK_ERROR;
	
}

int	process_number()
{
	//TODO: improve solution for larger numbers
	//TODO: a question: is overflow/underflow taken as a scanner error ?
	char buff[256] = {0, };
	int c,i = 0;

	enum numberType {INT, DOT,DOUBLE,EXP,EXP_SIGN,EXP_RADIX};
	int state = INT;
	while((c = fgetc(fHandle)) != EOF)
	{
		switch(state)
		{
			case INT:
				if(isdigit(c) || tolower(c) == 'e' || c == '.')
				{
					buff[i++] = c;
					if(c == '.')
						state = DOT;
					else if(tolower(c) == 'e')
						state = EXP;
				} else {
					ungetc(c,fHandle);
					g_lastToken.type = TOK_CONST;
					g_lastToken.data.integer = atoi(buff);
					return TOK_CONST;
				}
				break;
			case DOT:
				if(isdigit(c))
				{
					buff[i++] = c;
					state = DOUBLE;
				} else {
					// emit error, number ends with '.' without any following digit
					return TOK_ERROR;
				}
				break;
			case DOUBLE:
				if(!isdigit(c) && tolower(c) != 'e')
				{
					// new float
					ungetc(c,fHandle);
					g_lastToken.type = TOK_DOUBLECONST;
					g_lastToken.data.real= atof(buff);
					return TOK_DOUBLECONST;
				} else {
					buff[i++] = c;	
					if(tolower(c) == 'e')
						state = EXP;
				}
				break;
			case EXP:
				if(c == '+' || c == '-')
				{
					buff[i++] = c;
					state = EXP_SIGN;
				} else if(isdigit(c))
				{
					buff[i++] = c;
					state = EXP_RADIX;
				} else {
					return TOK_ERROR;
				}
				break;
			case EXP_SIGN:
				if(isdigit(c))
				{
					buff[i++] = c;
					state = EXP_RADIX;
				} else {
					return TOK_ERROR;
				}
				break;
			case EXP_RADIX:
				if(isdigit(c) || c == '+' || c == '-')
					buff[i++] = c;
				else 
				{
					//new float
					ungetc(c,fHandle);
					g_lastToken.type = TOK_DOUBLECONST;
					g_lastToken.data.real= atof(buff);
					return TOK_DOUBLECONST;
				}
				break;
				
		}
	}

	if (i) {
		switch (state){
			case INT:
				g_lastToken.type = TOK_CONST;
				g_lastToken.data.integer = atoi(buff);
				return TOK_CONST;
				break;
			case DOUBLE:
				g_lastToken.type = TOK_DOUBLECONST;
				g_lastToken.data.real= atof(buff);
				return TOK_DOUBLECONST;
				break;

			case EXP_RADIX:
				g_lastToken.type = TOK_DOUBLECONST;
				g_lastToken.data.real= atof(buff);
				return TOK_DOUBLECONST;
				break;
		}
	}

	return TOK_ERROR;
}

int	process_operator(char op)
{
	switch(op)
	{
		case '*':
			g_lastToken.type = TOK_MUL;		
			break;
		case '/':
			g_lastToken.type = TOK_DIV;		
			break;
		case '+':
			g_lastToken.type = TOK_PLUS;		
			break;
		case '-':
			g_lastToken.type = TOK_MINUS;		
			break;
		default:
			fprintf(stderr,"Unknown operator '%c'\n",op);
			return TOK_ERROR;
	}
	g_lastToken.data.op= op;
	return g_lastToken.type;
}

int	process_relation(char c)
{
	int nextc = fgetc(fHandle);
	// TODO: what exactly should we do upon receiving an EOF ?
	if(nextc == EOF)
		return TOK_ERROR;
	switch(c)
	{
		case '=':
			if(nextc == '=')
			{
				g_lastToken.type = TOK_EQ;
			} else {
				// assigment
				ungetc(nextc,fHandle);
				g_lastToken.type = TOK_ASSIGN;
			}
			return g_lastToken.type;
			break;
		case '!':
			if(nextc == '=')
				g_lastToken.type = TOK_NOTEQ;
			else
				return TOK_ERROR;
			return g_lastToken.type;
			break;
		case '<':
			if(nextc == '=')
				g_lastToken.type = TOK_LE;
			else {
				ungetc(nextc,fHandle);
				g_lastToken.type = TOK_LESS;
			}
			return g_lastToken.type;
			break;
		case '>':
			if(nextc == '=')
				g_lastToken.type = TOK_GE;
			else {
				ungetc(nextc,fHandle);
				g_lastToken.type = TOK_GREATER;
			}
			return g_lastToken.type;
			break;
		default:
			break;	
	}
	return TOK_ERROR;
}
// maps ASCII symbols to token types
int	process_symbol(char op)
{
	int type = 0;
	switch(op)
	{
		case '(':
			type = TOK_LEFT_PAR;
			break; 
		case ')':
			type = TOK_RIGHT_PAR;
			break; 
		case '{':
			type = TOK_LEFT_BRACE;
			break; 
		case '}':
			type = TOK_RIGHT_BRACE;
			break; 
		case ';':
			type = TOK_DELIM;
			break; 
		case ',':
			type = TOK_LIST_DELIM;
			break;
		default:
			return TOK_ERROR;
	}		
	g_lastToken.type = type;
	return g_lastToken.type;
}

// isBlock 	if func should process block comment
int	process_comments(int isBlock)
{
	// These states are used while waiting for a sequence to terminate
	// block commentary
	enum waitingState {FOR_END, NORMAL};
	enum commentaryType {LINE,BLOCK};
	int c,state = NORMAL;
	while((c = fgetc(fHandle)) != EOF)
	{
		switch(isBlock)
		{
			case LINE:
				// loop through text untill the end of line 
				if(c == '\n')
					return g_lastToken.type;
				break;
			case BLOCK:
				// either await a part of commentary or
				// wait for '/' in FOR_END after receiving '*'
				switch(state)
				{
					case NORMAL:
						if(c == '*')
							state = FOR_END;
						break;
					case FOR_END:
						if(c == '/')
							return g_lastToken.type;
						else if(c == '*')
							break;
						else
							state = NORMAL;
						break;
				}
		}
	}
	return TOK_ERROR;	
}

int	intern_getToken()
{
	// let's get a character from source code's stream
	int c;
	while((c = fgetc(fHandle)) != EOF)
	{
		// if the stream is at its end, either return EOF token
		// or report error (multiple EOF tokens reached)
		if(c == EOF)
			break;
		switch(c)
		{
			// literals start with "
			case '\"':
				ungetc(c,fHandle);
				return  process_literal();
				break;
			// skip these chars
			case ' ':
			case '\t': 
			case '\n':
				break;
			// start of commentaries or / operator
			case '/':
				c = fgetc(fHandle);
				switch(c)
				{
					case EOF:
						break;
					case '/':
					case '*':
						process_comments(c == '*');
						break;
					default:
						// it was an operator /
						ungetc(c,fHandle);
						return process_operator('/');
				}
				break;
			// math operators
			case '+':
			case '-':
			case '*':
				return process_operator(c);	
			case '{':
			case '}':
			case '(':
			case ')':
			case ';':
			case ',':
				return process_symbol(c);
			case '<':
			case '>':
			case '=':
			case '!':
				return process_relation(c);
			default:
				if(isalpha(c) || c == '_' || c == '$')
				{
					ungetc(c,fHandle);
					return process_identifier();
				} else if(isdigit(c))
				{
					ungetc(c,fHandle);
					return process_number();
				} else {
					fprintf(stderr,"Error: No token defined for 0x%X character\n",c);
					return TOK_ERROR;
				}
				break;
				
		}
	}
	// if there has already been an EOF token, report ERROR
	if(g_lastToken.type == TOK_EOF)
		return TOK_ERROR;

	// report the first occurence of EOF
	g_lastToken.type = TOK_EOF;
	return TOK_EOF;
}


int getToken()
{
	// special case -> avoid TOK_EOF multiplication
	if(currentToken != NULL && currentToken->token.type == TOK_EOF)
		return TOK_ERROR;
	// special case for n+1th pass
	if(currentToken == NULL && tokenList != NULL)
	{
		currentToken = tokenList;
		g_lastToken = currentToken->token;
		return g_lastToken.type;
	}
	// if we don't have any token remaining in our linked list
	if(currentToken == NULL || currentToken->next == NULL)
	{
		// then process new from file
		t_tokenElem* newel = malloc(sizeof(t_tokenElem));
		if(newel)
		{
			int ret = intern_getToken();	
			newel->prev = currentToken;
			newel->next = NULL;
			newel->token = g_lastToken;
			if(currentToken)
				currentToken->next = newel;
			currentToken = newel;
			if(!tokenList)
				tokenList = newel;
			return ret;
		}
		// malloc error
		// TODO: global error module
		
	} 
	// otherwise move to the next in linked list
	currentToken = currentToken->next;	
	g_lastToken = currentToken->token;
	return g_lastToken.type;
}
