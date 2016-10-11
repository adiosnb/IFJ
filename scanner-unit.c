#include "scanner.h"

int main(int argc, char* argv[])
{
	char name[256] = "source.java";
	if(argc >= 2)
		snprintf(name,255,"%s",argv[1]);

	int result = scanner_openFile(name);
	if(result)
	{
		while(getToken() != ERROR)
		{
			static int count = 0;
			count++;
			printf("%3d. ",count);
			switch(g_lastToken.type)
			{
				case TOK_OPERATOR:
					printf("Operator %c\n",g_lastToken.data.op);
					break;
				case TOK_ID:
					printf("ID %s\n",g_lastToken.data.string);
					break;
				case TOK_SPECIAL_ID:
					printf("SPECIALID %s\n",g_lastToken.data.string);
					break;
				case TOK_KEYWORD:
					printf("KEYWORD %s\n",g_lastToken.data.string);
					break;
				case TOK_CONST:
					printf("NUM %d\n",g_lastToken.data.integer);
					break;
				case TOK_DOUBLECONST:
					printf("DOUBLE %f\n",g_lastToken.data.real);
					break;
				case TOK_LITERAL:
					printf("LITERAL '%s' \n",g_lastToken.data.string);
					break;
				case TOK_RIGHT_BRACE:
					printf("}\n");
					break;
				case TOK_LEFT_BRACE:
					printf("{\n");
					break;
				case TOK_RIGHT_PAR:
					printf(")\n");
					break;
				case TOK_LEFT_PAR:
					printf("(\n");
					break;
				case TOK_LE:
					printf("<=\n");
					break;
				case TOK_GE:
					printf(">=\n");
					break;
				case TOK_EOF:
					printf("EOF\n");
					break;
				case TOK_DELIM:		// ;
					printf(";\n");
					break;
				case TOK_LIST_DELIM:	// ,
					printf(",\n");
					break;
				case TOK_ASSIGN:	// =
					printf("=\n");
					break;
				case TOK_EQ:		// ==
					printf("==\n");
					break;
				case TOK_NOTEQ:		// !=
					printf("!=\n");
					break;
				case TOK_LESS:		// <
					printf("<\n");
					break;
				case TOK_GREATER:	// >
					printf(">\n");
					break;
				default:
					printf("Unk token with type %d\n",g_lastToken.type);
					break;
			}
		} 
		if(g_lastToken.type != TOK_EOF)
		{
			printf("Lexical error has occured.\n");
			return 1;
		}
	} else {
		printf("Error has occured while opening the file %s.\n",name);	
	}
	return 0;
}
