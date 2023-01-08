/// \file

#include "heap.h"
#include "page.h"
#include "gc.h"

#ifndef __mover__
#define __mover__

/// @brief the acctual garbage collection
/// @param i_heap the internal heap
/// @param stack_ptrs pointers from the stack
/// @param ptrs_len number of pointers from the stack
/// @param unsafe_stack if the stack is to be considerd safe or not
/// @param proof_reading_arr checks if a pointer is equal to its proof reading pointer
void move (internal_heap_t *i_heap, void ***stack_ptrs, int ptrs_len, bool unsafe_stack, void **proof_reading_arr);

/// @brief moves data from a page to a new page
/// @param data_ptr used to get the metadata
/// @param new_pages pages to move to
/// @param len number of new pages
/// @param static_pages static pages
/// @param static_len number of static pages
/// @param i_heap the internal heap
void do_move (void **data_ptr, page_t **new_pages, int len, page_t **static_pages, int static_len, internal_heap_t *i_heap);

#endif
