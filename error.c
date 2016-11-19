#include <stdlib.h>
#include <stdarg.h>
#include "error.h"
#include "scanner.h"
#include "stable.h"

extern stab_t* staticSym; 


// Call module dtors
void clean_up()
{
	// TODO: 
	// scanner clean up
	scanner_closeFile();
	// symbolic table clean up
	stable_destroy(&staticSym);
	// interpret clean up
}


// Exit the process with 'errtype' return value and cleanup resources before leaving
void errorLeave(int errtype)
{
	clean_up();
	exit(errtype);
}

void error_and_die(enum errorTypes type, const char *fmt, ...)
{
    va_list valist;
    char buffer[512];
    
    va_start(valist, fmt);
    vsprintf(buffer, fmt, valist);
    fprintf(stderr, "%.511s\n", buffer);

    va_end(valist);

    // TODO: free all resources and end the program 
    errorLeave(type);
}

