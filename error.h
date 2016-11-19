// Have a look at page no.2 of assignment 
enum errorTypes
{
	// Lex. error (bad float format)
	LEXICAL_ERROR = 1,
	// Syntax error (violation of syntax rules)
	SYNTAX_ERROR = 2,
	// Undefined class/function/var, reinitialization of class, etc.
	SEMANTIC_ERROR = 3,
	// Semantic incompatibility of types (function parameters, function values)
	SEMANTIC_TYPE_ERROR = 4,
	// The rest
	SEMANTIC_ERROR_REST = 6,
	// Error occured during IO operation with built-in func. readDouble(), etc.
	RUNTIME_READ_ERROR = 7,
	// Work with uninitialized variable
	RUNTIME_UNINITIALIZED = 8,
	// Null division
	RUNTIME_NULLDIVISION = 9,
	// The rest of run-times
	RUNTIME_ERROR = 10,
	// Internal error (malloc failure,etc)
	INTERNAL_ERROR = 99

};

// Call module dtors
void clean_up();


// Leave the process with error 'errtype'
void errorLeave(int errtype);
