#include "heap.h"

struct internal_heap
{
  page_t **page_arr;
  size_t num_pages;
  
};
 
void *h_alloc_struct_internal(internal_heap_t *h, char *layout)
{
  //STUB: TODO!
  return NULL;  
}

void *h_alloc_data_internal(internal_heap_t *h, size_t bytes)
{
  //STUB: TODO!
  return NULL;  
}

internal_heap_t *h_init_internal(size_t bytes)
{
  //STUB: TODO!
  return NULL;
}

void h_delete_internal(internal_heap_t *h)
{
  //STUB: TODO!
}
