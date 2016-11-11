#ifndef _DYNAMIC_STACK_H
#define _DYNAMIC_STACK_H

#include <sys/types.h> 
#include <stdbool.h>

typedef  int  obj_t;
extern const size_t INIT_SIZE;  

typedef struct {
    obj_t   *elem;
    long int     top;
    size_t  size;
} stack_t;

stack_t stack_ctor(void);
bool stack_empty(const stack_t *const );
obj_t stack_top(const stack_t *const);
void stack_pop(stack_t *const);
void stack_push(stack_t *const, int c);
stack_t stack_dtor(stack_t *const stack);


#endif

