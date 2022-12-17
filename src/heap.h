#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include "page.h"

#ifndef __heap__
#define __heap__

typedef struct internal_heap internal_heap_t;

internal_heap_t *h_init_internal(unsigned int No_pages, unsigned int page_size);

void h_delete_internal(internal_heap_t *h);

void *h_alloc_struct_internal(internal_heap_t *h, char *layout);

void *h_alloc_data_internal(internal_heap_t *h, unsigned int bytes);

bool is_valid_ptr(internal_heap_t *h, void *ptr);

size_t get_internal_heap_actual_size(internal_heap_t *h);

page_t **get_passive_pages(internal_heap_t *h, int *len);

page_t **get_active_pages(internal_heap_t *h, int *len);

#endif
