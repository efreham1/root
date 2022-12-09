#include "heap.h"
#include "page.h"

struct internal_heap
{
  page_t **page_arr;
  size_t num_pages;
  size_t page_size;
};

internal_heap_t *h_init_internal(size_t bytes, size_t page_size)
{
  internal_heap_t *new_iheap = calloc(1,sizeof(internal_heap_t));

  new_iheap->num_pages = (bytes / page_size) + 1;

  new_iheap->page_size = page_size;
  
  new_iheap->page_arr = calloc(new_iheap->num_pages,sizeof(page_t *));
  for(int i = 0; i < new_iheap->num_pages; i++)
    {
      new_iheap->page_arr[i] = page_init(page_size);
    }
  return new_iheap;
}

void h_delete_internal(internal_heap_t *h)
{
  for(int i = 0; i < h->num_pages; i++)
    {
      page_delete(h->page_arr[i]);
    }
  free(h->page_arr);
  free(h);
}

void *h_alloc_struct_internal(internal_heap_t *h, char *layout)
{
  //STUB: TODO!
  return NULL;  
}

void *h_alloc_data_internal(internal_heap_t *h, size_t bytes)
{
  for(int i = 0; i < h->num_pages; i++)
    {
      if(has_room(h->page_arr[i], bytes))
	{
	  return page_alloc(h->page_arr[i],bytes);
	}
    }

  for(int i = 0; i < h->num_pages; i++)
  {
    if(!isActive(h->page_arr[i]))
      {
	make_active(h->page_arr[i]);
	
	return page_alloc(h->page_arr[i],bytes);
      }
  }
  
  
  return NULL;  
}
