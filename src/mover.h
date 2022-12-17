#include "heap.h"
#include "page.h"
#include "gc.h"

void move(heap_t *ext_heap);

void do_move(void **data_ptr, page_t **new_pages, int len);
