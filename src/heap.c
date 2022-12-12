#include "heap.h"
#include "page.h"
#include <stdio.h>

struct internal_heap
{
  page_t **page_arr;
  unsigned int num_pages;
  unsigned int page_size;
};

internal_heap_t *h_init_internal(unsigned int No_pages, unsigned int page_size)
{
  assert(No_pages >= 2);
  internal_heap_t *new_iheap = calloc(1, sizeof(internal_heap_t));
  
  new_iheap->num_pages = No_pages;
  new_iheap->page_size = page_size;
  new_iheap->page_arr = calloc(No_pages, sizeof(page_t *));
  
  for (int i = 0; i < new_iheap->num_pages; i++)
  {
    new_iheap->page_arr[i] = page_init(page_size);
  }
  return new_iheap;
}

void h_delete_internal(internal_heap_t *h)
{
  for (int i = 0; i < h->num_pages; i++)
  {
    page_delete(h->page_arr[i]);
  }
  free(h->page_arr);
  free(h);
}

void *h_alloc_struct_internal(internal_heap_t *h, char *layout)
{
  // STUB: TODO!
  return NULL;
}

void *h_alloc_data_internal(internal_heap_t *h, unsigned int bytes)
{
  for (int i = 0; i < h->num_pages; i++)
  {
    if (has_room(h->page_arr[i], bytes))
    {
      return page_alloc_data(h->page_arr[i], bytes);
    }
  }

  for (int i = 0; i < h->num_pages; i++)
  {
    if (!is_active(h->page_arr[i]))
    {
      make_active(h->page_arr[i]);

      return page_alloc_data(h->page_arr[i], bytes);
    }
  }

  return NULL;
}
