// A definition of dynamically growing string
// To be used in lexical analysis

/* How to use this type 

	t_dstring str;
	dstring_init(&str);
	dstring_append(&str,'A');
	dstring_concat(&str,"hoj");
	char* result = dstring_export(&str);
	dstring_free(&str);

NOTE: this type is useful for handling strings without knowing the final
length of string.
 */


// TODO: implementation
typedef struct 
{
	// linked list ?
} t_dstring;

// Inicialize an empty dynamicString
void dstring_init(t_dstring* t);

// concatenate t+str
// return 1 on success, 0 on fail
int dstring_concat(t_dstring* t, char* str);

// Append new char at the end of t
int dstring_append(t_dstring* t, char c);

// Create a null-terminated copy of dynamic string
char*	dstring_export(t_dstring* t);

// Free resources
void dstring_free(t_dstring* t);
