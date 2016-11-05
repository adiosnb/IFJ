#include <stdio.h>

#include "scanner/scanner.h"
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

    if (a == TOK_ERROR)
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

    stack_t pda = stack_ctor();
    int c =  edit_const_id_span(getToken());

    stack_push(&pda, DOLLAR);
//    printf("Push: '%s'\n", tokens[stack_top(&pda)]);
    do
    {
        int result = compare_token(stack_top(&pda), c);
        printf("Precedence: %s %c %s\n", tokens[stack_top(&pda)], result, tokens[c]);
        switch(result)
        {
                case '<':
                case '=':
                        stack_push(&pda, c);
                        printf("Input token: %s (%i)\n", tokens[c], c);
                        printf("Push: %s\n", tokens[stack_top(&pda)]);
                        print_stack(&pda);                        
                        c =  edit_const_id_span(getToken());
                        break;
                case '>':
//			printf("%s", tokens[stack_top(&pda)]);
                        printf("Pop: %s\n", tokens[stack_top(&pda)]);
                        stack_pop(&pda);
                        print_stack(&pda);                        
                        break; 
                case '_':
                        fprintf(stderr, "Expression syntax error: %s\n", tokens[stack_top(&pda)]); return; 
        }
    }
    while (c != DOLLAR || stack_top(&pda) != DOLLAR);
    printf("Input: %s\n", tokens[c]);
    printf("Stack: %s\n", tokens[stack_top(&pda)]);

}

int main(void)
{
	parse_expression();
	putchar('\n');
	return 0;
}
