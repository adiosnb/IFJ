#include "error.h"
#include <stdarg.h>
#include <stdio.h>

void error_and_die(enum error_type type, const char *fmt, ...)
{
    va_list valist;
    char buffer[512];
    
    va_start(valist, fmt);
    vsprintf(buffer, fmt, valist);
    fprintf(stderr, "%.511s\n", buffer);

    va_end(valist);

    // TODO: free all resources and end the program 
    exit(type);
}
    
