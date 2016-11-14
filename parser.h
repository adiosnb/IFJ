//-----------------------------------------------------------------------------
//	Private variables
//-----------------------------------------------------------------------------
// Parser state flag: are we parsing inside of a function ?
// NOTE: used to treat declaration of function in function as an error
int inFunction = 0;
#define setInFunction(st) inFunction = st

// Parser state flag: are we inside of a function call ? 
// NOTE: used to differentiate between formal parameters and arguments
int inCall = 0;
#define setInCall(st) inCall= st

// Parser state flag: how many compound-statements have we dive in so far ?
// NOTE: used to block any definition inside { } 
int ident= 0;
#define incIdent() ident++;
#define decIdent() ident--;

// Return state
// NOTE: upon using a error.c, this might actually be obsolete
enum syntaxCorrectness {SYN_OK,SYN_ERR};

//#define throw(format,...) fprintf(stderr,"[%s:%d]: "format"\n", __FILE__,__LINE__,##__VA_ARGS__),SYN_ERR
#define throw(format,...) fprintf(stderr,"[input:%d:%d] [%s:%d] "format"\n", getTokLine(),getTokTabs(),__FILE__,__LINE__,##__VA_ARGS__),SYN_ERR

//-----------------------------------------------------------------------------
//	DEBUGING MESSAGES
//-----------------------------------------------------------------------------
#if DEBUG > 0
#define GEN(format,...) fprintf(stderr,"[Generate:%d:%d]: "format"\n", getTokLine(), getTokTabs(),##__VA_ARGS__)
#else
#define GEN(format,...)  
#endif

#if DEBUG > 1
#define hint(format,...) fprintf(stderr,"[Hint:%d:%d]: "format"\n", getTokLine(), getTokTabs(),##__VA_ARGS__)
#else
#define hint(format,...)  
#endif

//-----------------------------------------------------------------------------
//	PUBLIC FUNCTIONS
//-----------------------------------------------------------------------------

int source_program();
