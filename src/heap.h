#include <stddef.h>

#ifndef __heap__
#define __heap__

typedef struct metadata metadata_t;
struct metadata
{
  const char *formatString;
  void *forwardingAdress; // If NULL => has not been copied!  
};

typedef struct internal_heap internal_heap_t;

internal_heap_t *h_init_internal(size_t bytes, size_t page_size);

void h_delete_internal(internal_heap_t *h);

void *h_alloc_struct_internal(internal_heap_t *h, char *layout);

void *h_alloc_data_internal(internal_heap_t *h, size_t bytes);

#endif
