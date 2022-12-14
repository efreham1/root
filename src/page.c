#include "page.h"
#include "metadata.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

struct page
{
  bool is_active;
  unsigned int size;
  unsigned int offset;
  void *memory_block;
  void *alloc_map;
};

size_t get_page_struct_size()
{
  return sizeof(page_t);
}

page_t *page_init(unsigned int bytes)
{
  assert(bytes >= 16 && bytes%8==0);
  page_t *newpage = calloc(1,sizeof(page_t));

  //int padding = (unsigned long)mem_block%8 == 0? 0: 8 - (unsigned long)mem_block%8;

  newpage->memory_block = calloc(bytes, 1);

  assert((unsigned long)newpage->memory_block%8 == 0);

  int alloc_map_size = (bytes/8-1)/8 + 1;
  newpage->alloc_map = calloc(alloc_map_size, 1);

  newpage->is_active = false;
  newpage->offset = 0;
  newpage->size = bytes;
  return newpage;
}

void set_bit_alloc_map(void *alloc_map, unsigned long bit_idx)
{
  assert(bit_idx >= 0);
  char *b = (char *) alloc_map;
  b[bit_idx/8] = b[bit_idx/8] | 1 << bit_idx%8;
}

bool get_bit_alloc_map(void *alloc_map, unsigned long bit_idx)
{
  assert(bit_idx >= 0);
  char *b = (char *) alloc_map;
  return b[bit_idx/8] >> bit_idx%8 & 1;
}

void page_delete(page_t *p)
{
  free(p->memory_block);
  free(p->alloc_map);
  free(p);
}

bool is_active(page_t *p) { return p->is_active ; }

void make_active(page_t *p) { p->is_active = true ; }

bool has_room(page_t *p, unsigned int bytes)
{
  if (!is_active(p))
  {
    return false;
  }

  unsigned int needed_size = bytes + sizeof(metadata_t);

  return(p->size - p->offset >= needed_size);
}

bool is_ptr_to_page(page_t *p, void *ptr)
{
  unsigned long bit_idx = (ptr - p->memory_block)/8 -1;
  return p->is_active && ptr > p->memory_block && ptr < p->memory_block + p->offset && get_bit_alloc_map(p->alloc_map, bit_idx);
}

void *page_alloc_data(page_t *page, unsigned int bytes)
{  
  metadata_t *metadata = (metadata_t *) (page->memory_block + page->offset);
  page->offset += sizeof(metadata_t);

  *metadata = set_data_size((unsigned int) bytes);

  unsigned long bit_idx = page->offset/8 - 1;
  set_bit_alloc_map(page->alloc_map, bit_idx);

  void *pointer = page->memory_block + page->offset;
  page->offset += bytes;
  return pointer;
}

void *page_alloc_struct(page_t *page, bool *format_vector, int len, unsigned int bytes)
{
  metadata_t *metadata = (metadata_t *) (page->memory_block + page->offset);
  page->offset += sizeof(metadata_t);

  *metadata = set_format_vector(format_vector, len);

  unsigned long bit_idx = page->offset/8 - 1;
  set_bit_alloc_map(page->alloc_map, bit_idx);

  void *pointer = page->memory_block + page->offset;
  page->offset += bytes;
  return pointer;
}