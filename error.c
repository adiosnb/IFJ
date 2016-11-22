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

const char* getErrorName(enum errorTypes type)
{
	switch(type)
	{
		case LEXICAL_ERROR:
			return "LEXICAL";
		case SYNTAX_ERROR:
			return "SYNTAX";
		case SEMANTIC_ERROR:
			return "SEMANTIC";
		case SEMANTIC_TYPE_ERROR:
			return "SEM.TYPE";
		case SEMANTIC_ERROR_REST:
			return "SEM.REST";
		case RUNTIME_READ_ERROR:
			return "RT_READ";
		case RUNTIME_UNINITIALIZED:
			return "RT_UINIT";
		case RUNTIME_NULLDIVISION:
			return "RT_NULL";
		case RUNTIME_ERROR:
			return "RUNTIME";
		case INTERNAL_ERROR:
			return "INTERNAL";
		default:
			break;
	}
	return "UNK";
}

void error_and_die(enum errorTypes type, const char *fmt, ...)
{
    va_list valist;
    
    va_start(valist, fmt);
    fprintf(stderr,"[%s][%d:%d] ",getErrorName(type),getTokLine(),getTokTabs());
    vfprintf(stderr,fmt, valist);
    fputc('\n',stderr);
    va_end(valist);

    errorLeave(type);
}

