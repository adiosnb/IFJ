#ifndef _ERROR_H
#define _ERROR_H

enum error_type
{
    ERR_LEX             = 1,
    ERR_SYNTAX          = 2,
    ERR_SEMANTIC_DEF    = 3,
    ERR_SEMANTIC_TYPE   = 4,
    ERR_SEMANTIC_OTHER  = 6,
    ERR_INPUT_TOKEN     = 7,
    ERR_UNINIT_VAR      = 8,
    ERR_ZERO_DIV        = 9,
    ERR_OTHERS          = 10,
    ERR_INTERPRET       = 99
};

void error_and_die(enum error_type, const char *, ...);

#endif
