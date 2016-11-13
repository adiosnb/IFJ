#include "error.h"
#include "scanner.h"

// Call module dtors
void clean_up()
{
	// TODO: 
	// scanner clean up
		scanner_closeFile();
	// symbolic table clean up
	// interpret clean up
}


// Exit the process with 'errtype' return value and cleanup resources before leaving
void errorLeave(int errtype)
{
	clean_up();
	exit(errtype);
}
