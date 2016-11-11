#include <stdio.h>

#include "scanner/scanner_alt.h"
#include "utils/dynamic_stack.h"
#include "error/error.h"

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

    // TODO: validate input symbol and print its string....how? scanner is not capable of doing that
    if (a >= TOK_EQ && a <= DOLLAR)
        return a;
    else
        error_and_die(ERR_SYNTAX, "Expression: invalid input symbol %s", g_lastToken.data.string);
    return -1;    
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
    "==", "!=", "<", ">", "<=", ">=", "+", "-", "*", "/", "i", "(", ")", "$"
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
    
    stack_t pda = stack_ctor();
    int c =  edit_const_id_span(getToken());
    int top;

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
                        do
                        {
                            top = stack_top(&pda);
                            if (top != TOK_LEFT_PAR && top != TOK_RIGHT_PAR)
                                printf("%s ", tokens[stack_top(&pda)]);
#if defined NDEBUG
                            printf("Pop: %s\n", tokens[stack_top(&pda)]);
#endif
                            stack_pop(&pda);
#if defined NDEBUG
                            print_stack(&pda);                        
#endif
                        }
                        while (compare_token(stack_top(&pda), top) != '<');

                        break; 
                case '_':
                        fprintf(stderr, "Expression syntax error: %s\n", tokens[stack_top(&pda)]); return; 
        }
    }
    while (c != DOLLAR || stack_top(&pda) != DOLLAR);

    putchar('\n');
#if defined NDEBUG
    printf("Input: %s\n", tokens[c]);
    printf("Stack: %s\n", tokens[stack_top(&pda)]);
#endif

}

int main(void)
{
	parse_expression();
	return 0;
}
