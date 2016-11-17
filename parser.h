//-----------------------------------------------------------------------------
//	Private variables
//-----------------------------------------------------------------------------
int isSecondPass = 0;

// Return state
// NOTE: upon using a error.c, this might actually be obsolete
enum syntaxCorrectness {SYN_OK,SYN_ERR};

//#define throw(format,...) fprintf(stderr,"[%s:%d]: "format"\n", __FILE__,__LINE__,##__VA_ARGS__),SYN_ERR
#define throw(format,...) fprintf(stderr,"[input:%d:%d] [%s:%d] "format"\n", getTokLine(),getTokTabs(),__FILE__,__LINE__,##__VA_ARGS__),SYN_ERR

#define PLACEHOLDER_CLASS	1337
#define PLACEHOLDER_STATIC	123
#define PLACEHOLDER_LOCAL 	777

#define UNINITIALIZED		666


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
