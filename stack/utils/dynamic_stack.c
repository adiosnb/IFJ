#include "dynamic_stack.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

const size_t INIT_SIZE = 512;
const double RESIZE_FACTOR = 0.7;

static stack_t stack_resize(stack_t *const, size_t);

stack_t stack_ctor(void)
{
    stack_t stack;

    stack.elem = calloc(sizeof(obj_t), INIT_SIZE);
    if (stack.elem == NULL)
        fprintf(stderr, "%s: Allocation of %zu elements for stack failed!\n", __func__, INIT_SIZE);
    stack.size = INIT_SIZE;
    stack.top = -1;

    return stack;
}

bool stack_empty(const stack_t *const stack)
{
    if (stack)
       return stack->top == -1; 
    return true;
}

obj_t stack_top(const stack_t *const stack)
{
    if (stack && stack->top >= 0)  
       return stack->elem[stack->top];
    return -1;
}

void stack_pop(stack_t *const stack)
{
    if (stack && stack->top >= 0)
       --stack->top;    
}

void stack_push(stack_t *const stack, int c)
{
    if (stack)
    {
       if (stack->top >= stack->size*RESIZE_FACTOR) 
           *stack = stack_resize(stack, stack->size + stack->size);
       stack->elem[++stack->top] = c;  
    }
}

static stack_t stack_resize(stack_t *const stack, size_t sz)
{
   obj_t *s = realloc(stack->elem, sz*sizeof(obj_t));

   if (s != NULL)
   {
       stack->elem = s;
       stack->size = sz;
   }
   else
       fprintf(stderr, "%s: Reallocation of %zu bytes for stack failed!\n", __func__, sz);
   return *stack;
}

stack_t stack_dtor(stack_t *const stack)
{
    if (stack)
    {
        if (stack->elem != NULL)
            free(stack->elem);
        stack->elem = NULL;
        stack->size = 0;
        stack->top = -1;
    }
    return *stack;
}
