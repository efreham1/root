#ifndef __page__
#define __page__

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

typedef struct page page_t;

size_t get_page_struct_size();

page_t *page_init(unsigned int bytes);

void page_delete(page_t *p);

bool is_active(page_t *p);

bool has_room(page_t *p, unsigned int bytes);

void *page_alloc_data(page_t *page, unsigned int bytes);

void *page_alloc_struct(page_t *page, bool *format_vector, int len, unsigned int bytes);

void make_active(page_t *p);

bool is_ptr_to_page(page_t *p, void *ptr);

#endif
