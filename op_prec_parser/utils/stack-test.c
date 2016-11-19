#include <stdio.h>
#include "dynamic_stack.h"
#include <assert.h>

int main(void)
{
   stack_t stack = stack_ctor(); 

   assert(stack_empty(&stack));

   stack_push(&stack, 10);
   stack_push(&stack, 20);
   stack_push(&stack, 30);
   
   assert(stack_top(&stack) == 30);    

   stack_pop(&stack);

   assert(stack_top(&stack) == 20);    

   stack_pop(&stack);

   assert(stack_top(&stack) == 10);    

   stack_pop(&stack);

   assert(stack_empty(&stack));

   // now lets try dynamic allocation
   for (size_t i = 0; i != 4096*100; i++)
      stack_push(&stack, i+1); 

   assert(stack_top(&stack) == 4096*100);

   for (size_t i = 0; i != 4096*100; i++)
      stack_pop(&stack); 

   stack = stack_dtor(&stack);

   assert(stack.elem == NULL && stack.top == -1 && stack.size == 0);

   return 0;
}


