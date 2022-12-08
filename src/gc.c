#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "gc.h"

struct external_heap
{
  internal_heap_t *heapPtr;
  bool unsafe_stack;  
  float gcTrigger;
};

heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
  heap_t *newHeap = calloc(1,sizeof(heap_t));
  newHeap->heapPtr = h_init_internal(bytes);
  newHeap->unsafe_stack = unsafe_stack;
  newHeap->gcTrigger = gc_threshold;

  return newHeap;
}

void h_delete(heap_t *h)
{
  h_delete_internal(h->heapPtr);
  free(h);
}

void *h_alloc_struct(heap_t *h, char *layout)
{
  h_alloc_struct_interal(h->heapPtr, layout);
}


void *h_alloc_data(heap_t *h, size_t bytes)
{
  h_alloc_data_interal(h->heapPtr, bytes);
}
