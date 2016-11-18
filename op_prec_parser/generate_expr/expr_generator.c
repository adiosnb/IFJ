#include <ctype.h>
#include <stdio.h>
#define __USE_MISC 
#define  __USE_XOPEN_EXTENDED
#include <stdlib.h>

// how long is generated expression
const int MAX_OUTPUT = 1000;

const int MAX_OPERATORS = 10;
const int MAX_IDENTIFIERS = 4;

const int MAX_RULES = 12;

#define MAX_DEPTH 1000 

const char *rules[] = 
{
    "C == C",
    "C != C",
    "C < C",
    "C > C",
    "C <= C",
    "C >= C",
    "C + C",
    "C - C",
    "C * C",
    "C / C",
    "(C)",
    "C"
};

const char *identifiers[] =
{ 
   "1",
   "ID",
   "1.5",
   "a.b"
};

const char *operators[] = 
{
   "==",
   "!=",
   "<",
   ">",
   "<=",
   ">=",
   "+",
   "-",
   "*",
   "/"
};

void generate_expr(const char *rule)
{
    static int depth = 2* MAX_DEPTH;

    for (; *rule != '\0'; rule++)
        if (*rule == 'C')
        {
            if (depth > 0)
            {
                depth--;
                generate_expr(rules[random() % MAX_RULES]);
            }
            else
                printf("%s ", identifiers[random() % MAX_IDENTIFIERS]);
        }
        else if (!isspace(*rule)) 
            printf("%c", *rule);

}

int main(void)
{

   generate_expr(rules[random() % (MAX_RULES-2)]);
   return 0;

}
