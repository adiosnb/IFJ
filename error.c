#include <stdlib.h>
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
