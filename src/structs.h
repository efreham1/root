/// \file

#include "heap.h"
#include "page.h"

#ifndef __structs__
#define __structs__

struct external_heap {
    internal_heap_t *internal_heap;
    bool unsafe_stack;
    float gcTrigger;
    unsigned int tot_size;
};

struct internal_heap {
    page_t **page_arr;
    unsigned int num_pages;
    unsigned int page_size;
    unsigned int num_active_pages;
    bool visitation_bit;
    void *memory_block;
    void *end_of_memory_block;
};

struct page {
    bool is_active;
    unsigned int size;
    unsigned int offset;
    void *memory_block;
    void *alloc_map;
};

#endif