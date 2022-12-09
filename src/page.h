#ifndef __gc__
#define __gc__

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct page page_t;

page_t *page_init(size_t bytes);

void page_delete(page_t *p);

bool isActive(page_t *p);

bool has_room(page_t *p, size_t bytes);


#endif
