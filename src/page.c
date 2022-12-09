#include "page.h"
#include "heap.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

struct page
{
  bool isActive;
  size_t size;
  unsigned int offset;
  void *memoryBlock;
};

page_t *page_init(size_t bytes)
{
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

bool isActive(page_t *p) { return p->isActive ; }

void make_active(page_t *p) { p->isActive = true ; }

bool has_room(page_t *p, size_t bytes)
{
  return(p->size - p->offset >= bytes  + sizeof(metadata_t) && isActive(p));
}

void *page_alloc(page_t *page, size_t bytes)
{
  metadata_t *metadata = (metadata_t *) page->memoryBlock + page->offset;
  page->offset += sizeof(metadata_t);
  metadata->formatString = strdup("");
  metadata->forwardingAdress = NULL;

  void *pointer = page->memoryBlock + page->offset;
  page->offset += bytes;
  return pointer;
}
