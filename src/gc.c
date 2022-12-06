#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "gc.h"

struct heap
{
  void **memoryBlock;
  bool unsafe_stack;
  float gcTrigger;
};

heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
  heap_t *newHeap = calloc(1,sizeof(heap_t));
  
  void **memBlock = calloc(bytes,1);
  newHeap->memoryBlock = memBlock;
  newHeap->unsafe_stack = unsafe_stack;
  newHeap->gcTrigger = gc_threshold;

  return newHeap;
}

void h_delete(heap_t *h)
{
  free(h->memoryBlock);
  free(h);
}

int main()
{
  // printf("size of a char: %ld \n", sizeof(char));
  
  heap_t *h = h_init(2,false,5.0);
  
  h_delete(h);
    
  return 0;
}
