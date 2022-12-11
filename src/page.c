#include "page.h"
#include "metadata.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

struct page
{
  bool isActive;
  unsigned int size;
  unsigned int offset;
  void *memoryBlock;
};

page_t *page_init(unsigned int bytes)
{
  assert(bytes > sizeof(page_t));
  bytes -= sizeof(page_t);
  page_t *newpage = calloc(1,sizeof(page_t));
  newpage->isActive = false;
  newpage->size = bytes;
  newpage->offset = 0;
  newpage->memoryBlock = calloc(bytes,1);
  return newpage;
}

void page_delete(page_t *p)
{
  free(p->memoryBlock);
  free(p);
}

bool is_active(page_t *p) { return p->isActive ; }

void make_active(page_t *p) { p->isActive = true ; }

bool has_room(page_t *p, unsigned int bytes)
{
  return(p->size - p->offset >= bytes  + sizeof(metadata_t) && is_active(p));
}

void *page_alloc(page_t *page, unsigned int bytes)
{
  metadata_t *metadata = (metadata_t *) (page->memoryBlock + page->offset);
  page->offset += sizeof(metadata_t);
  *metadata = set_data_size((unsigned int) bytes);

  void *pointer = page->memoryBlock + page->offset;
  page->offset += bytes;
  return pointer;
}
