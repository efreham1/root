#ifndef __gc__
#define __gc__

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

typedef struct page page_t;

page_t *page_init(unsigned int bytes);

void page_delete(page_t *p);

bool is_active(page_t *p);

bool has_room(page_t *p, unsigned int bytes);

void *page_alloc(page_t *page, unsigned int bytes);

void make_active(page_t *p);

#endif
