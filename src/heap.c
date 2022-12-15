#include "heap.h"
#include "page.h"
#include <stdio.h>
#include <string.h>

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

void *h_alloc_struct_internal(internal_heap_t *h, char *format_string)
{
  unsigned int bytes = 0;
  bool format_vector[strlen(format_string)];
  size_t size = 0;
  int idx = 0;
  for (char *c = format_string; *c != '\0'; c++)
  {
    switch (*c)
    {
    case 'c':
      size = sizeof(char);
      break;
    
    case 'i':
      size = sizeof(int);
      break;
      
    case 'f':
      size = sizeof(float);
      break;

    case '*':
      size = sizeof(void *);
      break;
    
    case 'd':
      size = sizeof(double);
      break;
    
    case 'l':
      size = sizeof(long);
      break;
    
    default:
      assert(false);
      break;
    }

    int padding = bytes%size == 0? 0: size - bytes%size;

    if ((bytes+padding)%8 == 0)
    {
      format_vector[idx++] = *c == '*';
    }

    bytes += padding + size;
  }

  int padding = bytes%8==0? 0: 8 - bytes%8;
  bytes += padding;

  for (int i = 0; i < h->num_pages; i++)
  {
    if (has_room(h->page_arr[i], bytes))
    {
      return page_alloc_struct(h->page_arr[i], format_vector, idx, bytes);
    }
  }

  for (int i = 0; i < h->num_pages; i++)
  {
    if (!is_active(h->page_arr[i]))
    {
      make_active(h->page_arr[i]);

      return page_alloc_struct(h->page_arr[i], format_vector, idx, bytes);
    }
  }

  return NULL;
}

void *h_alloc_data_internal(internal_heap_t *h, unsigned int bytes)
{
  int padding = bytes%8 == 0? 0: 8 - bytes%8;
  bytes = bytes >= 8? bytes + padding: 8;
  
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

bool is_valid_ptr(internal_heap_t *h, void *ptr)
{
  for (int i = 0; i < h->num_pages; i++)
  {
    if (is_ptr_to_page(h->page_arr[i], ptr))
    {
      return true;
    }
  }
  return false;
}

size_t get_internal_heap_actual_size(internal_heap_t *h)
{
  size_t sum = 0;

  for (size_t i = 0; i < h->num_pages; i++)
  {
    sum += get_page_actual_size(h->page_arr[i]);
  }
  
  return sizeof(internal_heap_t) + h->num_pages*sizeof(page_t *) + sum;
}