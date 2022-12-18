#include "page.h"
#include "metadata.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "structs.h"

size_t get_page_actual_size(page_t *p)
{
  return sizeof(page_t) + (p->size / 8 - 1) / 8 + 1 + p->size;
}

page_t *page_init(unsigned int bytes, void *page_start)
{
  assert(bytes >= 16 && bytes % 8 == 0);
  page_t *newpage = calloc(1, sizeof(page_t));

  // int padding = (unsigned long)mem_block%8 == 0? 0: 8 - (unsigned long)mem_block%8;

  newpage->memory_block = page_start;

  assert((unsigned long)newpage->memory_block % 8 == 0);

  int alloc_map_size = (bytes / 8 - 1) / 8 + 1;
  newpage->alloc_map = calloc(alloc_map_size, 1);

  newpage->is_active = false;
  newpage->offset = 0;
  newpage->size = bytes;
  return newpage;
}

void set_bit_alloc_map(void *alloc_map, unsigned long bit_idx)
{
  assert(bit_idx >= 0);
  char *b = (char *)alloc_map;
  b[bit_idx / 8] = b[bit_idx / 8] | 1 << bit_idx % 8;
}

bool get_bit_alloc_map(void *alloc_map, unsigned long bit_idx)
{
  assert(bit_idx >= 0);
  char *b = (char *)alloc_map;
  return b[bit_idx / 8] >> bit_idx % 8 & 1;
}

void page_delete(page_t *p)
{
  free(p->alloc_map);
  free(p);
}

bool is_active(page_t *p) { return p->is_active; }

void make_active(page_t *p) { p->is_active = true; }

void make_passive(page_t *p)
{
  p->is_active = false;
  p->offset = 0;

  int alloc_map_size = (p->size / 8 - 1) / 8 + 1;
  char *alloc_map = p->alloc_map;
  for (size_t i = 0; i < alloc_map_size; i++)
  {
    alloc_map[i] = 0;
  }
}

bool has_room(page_t *p, unsigned int bytes)
{
  if (!is_active(p))
  {
    return false;
  }

  unsigned int needed_size = bytes + sizeof(metadata_t);

  return (avail_space_page(p) >= needed_size);
}

bool is_ptr_to_page(page_t *p, void *ptr)
{
  unsigned long bit_idx = (ptr - p->memory_block) / 8 - 1;
  return p->is_active && ptr > p->memory_block && ptr < p->memory_block + p->offset && get_bit_alloc_map(p->alloc_map, bit_idx);
}

metadata_t *alloc_metadata(page_t *page)
{
  assert(page->offset < page->size - sizeof(metadata_t));

  metadata_t *md = (metadata_t *)(page->memory_block + page->offset);

  page->offset += sizeof(metadata_t);

  return md;
}

void *alloc_mem_block(page_t *page, unsigned int bytes)
{
  assert(avail_space_page(page) >= bytes);

  unsigned long bit_idx = page->offset / 8 - 1;
  set_bit_alloc_map(page->alloc_map, bit_idx);

  void *pointer = page->memory_block + page->offset;
  page->offset += bytes;
  return pointer;
}

void *page_alloc_data(page_t *page, unsigned int bytes)
{
  assert(bytes % 8 == 0);

  metadata_t *metadata = alloc_metadata(page);

  *metadata = set_data_size((unsigned int)bytes);

  return alloc_mem_block(page, bytes);
}

void *page_alloc_struct(page_t *page, bool *format_vector, int len, unsigned int bytes)
{
  assert(bytes % 8 == 0);

  metadata_t *metadata = alloc_metadata(page);

  *metadata = set_format_vector(format_vector, len);

  return alloc_mem_block(page, bytes);
}

unsigned int avail_space_page(page_t *p)
{
  return p->size - p->offset;
}

unsigned int used_space_page(page_t *p)
{
  return p->offset;
}