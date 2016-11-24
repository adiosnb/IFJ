#include <stdio.h>
#include "dynamic_stack.h"
#include <assert.h>

int main(void)
{
   dstack_t stack = dstack_ctor(); 

   assert(dstack_empty(&stack));

   dstack_push(&stack, 10);
   dstack_push(&stack, 20);
   dstack_push(&stack, 30);
   
   assert(dstack_top(&stack) == 30);    

   dstack_pop(&stack);

   assert(dstack_top(&stack) == 20);    

   dstack_pop(&stack);

   assert(dstack_top(&stack) == 10);    

   dstack_pop(&stack);

   assert(dstack_empty(&stack));

   // now lets try dynamic allocation
   for (size_t i = 0; i != 4096*100; i++)
      dstack_push(&stack, i+1); 

   assert(dstack_top(&stack) == 4096*100);

   for (size_t i = 0; i != 4096*100; i++)
      dstack_pop(&stack); 

   stack = dstack_dtor(&stack);

   assert(stack.elem == NULL && stack.top == -1 && stack.size == 0);

   return 0;
}


