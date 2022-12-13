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
  void * memory_block;
};

size_t get_page_struct_size()
{
  return sizeof(page_t) + 8;
}

page_t *page_init(unsigned int bytes)
{
  assert(bytes >= 16);
  page_t *newpage = calloc(1,sizeof(page_t));
  newpage->is_active = false;
  void *mem_block = calloc(bytes+8, 1);
  int padding = (unsigned long)mem_block%8 == 0? 0: 8 - (unsigned long)mem_block%8;
  newpage->memory_block = mem_block;
  newpage->offset = padding;
  newpage->size = bytes + padding;
  return newpage;
}

void page_delete(page_t *p)
{
  free(p->memory_block);
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

void *page_alloc_data(page_t *page, unsigned int bytes)
{  
  metadata_t *metadata = (metadata_t *) (page->memory_block + page->offset);
  *metadata = set_data_size((unsigned int) bytes);
  page->offset += sizeof(metadata_t);

  void *pointer = page->memory_block + page->offset;
  page->offset += bytes;
  return pointer;
}

void *page_alloc_struct(page_t *page, bool *format_vector, int len, unsigned int bytes)
{
  metadata_t *metadata = (metadata_t *) (page->memory_block + page->offset);
  page->offset += sizeof(metadata_t);

  *metadata = set_format_vector(format_vector, len);

  void *pointer = page->memory_block + page->offset;
  page->offset += bytes;
  return pointer;
}