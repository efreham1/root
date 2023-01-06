/// \file

#include "heap.h"
#include "page.h"
#include "gc.h"

#ifndef __mover__
#define __mover__

void move(internal_heap_t *i_heap, void ***stack_ptrs, int ptrs_len, bool unsafe_stack, void **proof_reading_arr);

void do_move(void **data_ptr, page_t **new_pages, int len, page_t **static_pages, int static_len, internal_heap_t *i_heap);

#endif