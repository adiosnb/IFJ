#include <ctype.h>
#include <stdlib.h>
#include "scanner.h"

FILE*	fHandle;

int	scanner_openFile(char* fileName)
{
	fHandle = fopen(fileName,"r");	
	if(fHandle)
		return 1;
	return 0;
}

int	scanner_closeFile()
{
	fclose(fHandle);
	return 0;
}

t_token	g_lastToken;

char*	createString(const char* str)
{
	char* nstr = malloc(strlen(str));
	if(nstr)
	{
		strcpy(nstr,str);
		return nstr;
	}
	return NULL;
}


// processes text literals such as "text" or "tex\t"
int	process_literal()
{
	fgetc(fHandle);
	// a normal state is used when awaiting regular ASCII input
	// SPECIAL state is reached after receiving '\' 
	enum States {NORMAL, SPECIAL};
	int c, state = NORMAL;

	// TODO: provide a better string datatype which would allow 'unlimited' strings
	char tempString[1001] = "\0";
	int i = 0;

	while((c = fgetc(fHandle)) != EOF)
	{
		switch(state)
		{
			case NORMAL:
				if(c == '\"')
				{
					// when the end of terminal is reached
					g_lastToken.type = TOK_LITERAL;
					g_lastToken.data.string = createString(tempString);
					// TODO: add pointer to symbol
					return OK;

				} else if(c == '\\')
				{
					state = SPECIAL;
					break;
				} else {
					tempString[i++] = c;
				}
				break;
			// process escape sequences (e.g. \n) or report invalid ones
			case SPECIAL:
				switch(c)
				{
					case '\\':
					case '\"':
						tempString[i++] = c;
						break;
					case 'n':
						tempString[i++] = '\n';
						break;
					case 't':
						tempString[i++] = '\t';
						break;
					default:

						// TODO: report an error - invalid escape sequence
						return ERROR;
						break;
				}
				state = NORMAL;
				break;
		}
	}
	return OK;
}

// Utility function
char*	isKeyword(const char* str)
{
	static char* keywords[] = {"boolean","break","class","continue","do","double",
	"else","false","for","if","int","return","String","static","true","void","while",NULL};

	for(int i = 0; keywords[i] != NULL; i++)
	{
		if(strcmp(keywords[i],str) == 0)
			return keywords[i];
	}
	return NULL;
}

int	process_identifier()
{
	char buff[256] = {0,};
	int i = 0,c;
	// nonAlpha is toggled to 1 if at least one character is non-alphanumerical
	// -> useful for skipping keyword comparing 
	int isNonAlpha = 0;
	while((c = fgetc(fHandle)) != EOF)
	{
		int res = isalnum(c);
	
		// if character fullfills ID requirements 
		if(res || c == '$' || c == '_')
		{
			isNonAlpha |= (res == 0);
			buff[i++] = c;
		} else {
			// the last character is probably a part of another token->return back
			ungetc(c,fHandle);
			// verify if string isn't a keyword
			if(!isNonAlpha)
			{
				char* key = isKeyword(buff);
				if(key)
				{
					g_lastToken.type = TOK_KEYWORD;
					g_lastToken.data.string = createString(buff);
					return OK;
				}
				
			}
			// otherwise, it's an ID
			g_lastToken.type = TOK_ID;
			g_lastToken.data.string = createString(buff);
			return OK;
		}
	}
	return ERROR;
	
}

int	process_number()
{
	//TODO: improve solution for larger numbers
	//TODO: a question: is overflow/underflow taken as a scanner error ?
	char buff[256] = {0, };
	int c,i = 0;

	enum numberType {INT, RADIX,EXP};
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
						state = RADIX;
					else if(tolower(c) == 'e')
						state = EXP;
				} else {
					// a new integer token
					// TODO:
					ungetc(c,fHandle);
					g_lastToken.type = TOK_CONST;
					g_lastToken.data.integer = atoi(buff);
					return OK;
				}
				break;
			case RADIX:
				if(!isdigit(c) && tolower(c) != 'e')
				{
					// new float
					ungetc(c,fHandle);
					g_lastToken.type = TOK_DOUBLECONST;
					g_lastToken.data.real= atof(buff);
					return OK;
				} else {
					buff[i++] = c;	
					if(tolower(c) == 'e')
						state = EXP;
					
				}
				break;
			case EXP:
				//TODO: only one +/-
				if(isdigit(c) || c == '+' || c == '-')
					buff[i++] = c;
				else 
				{
					//new float
					ungetc(c,fHandle);
					g_lastToken.type = TOK_DOUBLECONST;
					g_lastToken.data.real= atof(buff);
					return OK;
				}
		}
	}
	return ERROR;
}

int	process_operator(char op)
{
	g_lastToken.type = TOK_OPERATOR;
	g_lastToken.data.op= op;
	return OK;
}

int	process_relation(char c)
{
	int nextc = fgetc(fHandle);
	// TODO: what exactly should we do upon receiving an EOF ?
	if(nextc == EOF)
		return ERROR;
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
			return OK;
			break;
		case '!':
			if(nextc == '=')
				g_lastToken.type = TOK_NOTEQ;
			else
				return ERROR;
		case '<':
			if(nextc == '=')
				g_lastToken.type = TOK_LE;
			else {
				ungetc(nextc,fHandle);
				g_lastToken.type = TOK_LESS;
				return OK;
			}
		case '>':
			if(nextc == '=')
				g_lastToken.type = TOK_GE;
			else {
				ungetc(nextc,fHandle);
				g_lastToken.type = TOK_GREATER;
			}
		default:
			break;	
	}
	return ERROR;
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
		default:
			return ERROR;
	}		
	g_lastToken.type = type;
	return OK;
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
					return OK;
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
							return OK;
						else if(c == '*')
							break;
						else
							state = NORMAL;
						break;
				}
		}
	}
	return ERROR;	
}

int	getToken()
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
						return process_operator('*');
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
				return process_symbol(c);
			case '<':
			case '>':
			case '=':
			case '!':
				return process_relation(c);
			case '.':
				//TODO: compound IDs
				break;
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
					return ERROR;
				}
				break;
				
		}
	}
	if(g_lastToken.type == TOK_EOF)
		return ERROR;
	else {
		g_lastToken.type = TOK_EOF;
		return OK;
	}
	return OK;
}
