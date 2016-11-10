#include <stdio.h>

#include "scanner/scanner_alt.h"
#include "utils/dynamic_stack.h"

const char op_table[][14] =
{
//       ==   !=    <	 >   <=   >=	+    -	  *    /   ID	 (    )	   $
/* == */{'>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' },
/* != */{'>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' },
/* <  */{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>' },
/* >  */{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>' },
/* <= */{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>' },
/* >= */{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>' },
/* +  */{'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '>' },
/* -  */{'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '>' },
/* *  */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>' },
/* /  */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '>' },
/* ID */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '_', '_', '>', '>' },
/* (  */{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '_' },
/* )  */{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '_', '_', '>', '>' },
/* $  */{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '_', '=' },
};

//#define NDEBUG
#define DOLLAR  13 
static inline int edit_const_id_span(int a)
{
   
    switch (a)
    {
        case TOK_SPECIAL_ID:
        case TOK_LITERAL:
        case TOK_CONST:
        case TOK_DOUBLECONST: a = TOK_ID;
    }

    if (a == TOK_EOF)
        a = DOLLAR;
    return a;
}


static char compare_token(int pda_symbol, int input_symbol)
{
    
//    if (a > TOK_RIGHT_PAR || a < 0)
//    {
//        fprintf(stderr, "Bad token '%c' !\n", a);
//        return -1;
//    }
//
//    if (b > TOK_RIGHT_PAR || b < 0)
//    {
//        fprintf(stderr, "Bad token '%c' !\n", b);
//        return -1;
//    }
//    fprintf(stderr,"TOK: %i\n", op_table[a][b]);
//     
    

//    printf("input_symbol: '%i'\n", input_symbol);

    return op_table[pda_symbol][input_symbol];
}

int a,b;
int result;
int temp = 40;

const char *tokens[] = 
{
    "==", "!=", "<", ">", "<=", ">=", "+", "-", "*", "/", "ID", "(", ")", "$"
};

void print_stack(stack_t *s)
{
    int c = 0; 

    while (c<= s->top)
        printf(" %s |", tokens[s->elem[c++]]);
    putchar('\n');
}

void parse_expression(void)
{
    if (!scanner_openFile("input_test.txt"))
    {
        fprintf(stderr, "Cannot open file!\n");
        return;
    }
    
    int x;
    while ((x = edit_const_id_span(getToken())) != TOK_EOF)
        printf("%s", tokens[x]);
    putchar('\n');
    return;

    stack_t pda = stack_ctor();
    int c =  edit_const_id_span(getToken());

    stack_push(&pda, DOLLAR);
//    printf("Push: '%s'\n", tokens[stack_top(&pda)]);
    do
    {
        int result = compare_token(stack_top(&pda), c);     

#if defined NDEBUG
        printf("Precedence: %s %c %s\n", tokens[stack_top(&pda)], result, tokens[c]);
#endif

        switch(result)
        {
                case '<':
                case '=':
                        stack_push(&pda, c);
#if defined NDEBUG
                        printf("Input token: %s (%i)\n", tokens[c], c);
                        printf("Push: %s\n", tokens[stack_top(&pda)]);
                        print_stack(&pda);                        
#endif
                        c =  edit_const_id_span(getToken());
                        break;
                case '>':
                        printf("%s", tokens[stack_top(&pda)]);
#if defined NDEBUG
                        printf("Pop: %s\n", tokens[stack_top(&pda)]);
#endif
                        stack_pop(&pda);
#if defined NDEBUG
                        print_stack(&pda);                        
#endif
                        break; 
                case '_':
                        fprintf(stderr, "Expression syntax error: %s\n", tokens[stack_top(&pda)]); return; 
        }
    }
    while (c != DOLLAR || stack_top(&pda) != DOLLAR);

#if defined NDEBUG
    printf("Input: %s\n", tokens[c]);
    printf("Stack: %s\n", tokens[stack_top(&pda)]);
#endif

}

int main(void)
{
	parse_expression();
	putchar('\n');
	return 0;
}
