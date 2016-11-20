#include <ctype.h>
#include <time.h>
#include <stdio.h>
#define __USE_MISC 
#define  __USE_XOPEN_EXTENDED
#include <stdlib.h>

// how long is generated expression
const int MAX_OUTPUT = 1000;

#define MAX_OPERATORS (sizeof(operators)/sizeof(char *))
#define MAX_IDENTIFIERS (sizeof(identifiers)/sizeof(char *))
#define MAX_RULES (sizeof(rules)/sizeof(char *))

#define MAX_DEPTH  20

const char *rules[] = 
{
  //  "C == C",
  //  "C != C",
  //  "C < C",
  //  "C > C",
  //  "C <= C",
  //  "C >= C",
    "C + C",
    "C - C",
    "C * C",
    "C / C",
    "(C)",
    "C"
};

const char *identifiers[] =
{ 
   "9",
   "ID",
   "1.5",
   "a.b"
};

const char *operators[] = 
{
 //  "==",
 //  "!=",
 //  "<",
 //  ">",
 //  "<=",
 //  ">=",
   "+",
   "-",
   "*",
   "/"
};

int counter = 0;

int rdtsc()    
{    
        __asm__ __volatile__("rdtsc");    
}    


void generate_expr(const char *rule)
{
    static int depth = MAX_DEPTH;

    srandom(rdtsc());

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

    counter++;

    depth++;

}

int main(void)
{

   generate_expr(rules[random() % (MAX_RULES-2)]);
   printf("\n");
   fprintf(stderr, "Count: %i\n", counter);
   return 0;

}
