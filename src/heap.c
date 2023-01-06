#include "heap.h"
#include <stdio.h>
#include <string.h>
#include "structs.h"

internal_heap_t *h_init_internal(unsigned int No_pages, unsigned int page_size)
{
  assert(No_pages >= 2);
  internal_heap_t *new_iheap = calloc(1, sizeof(internal_heap_t));

  new_iheap->num_pages = No_pages;
  new_iheap->page_size = page_size;
  new_iheap->page_arr = calloc(No_pages, sizeof(page_t *));
  new_iheap->memory_block = calloc(No_pages * page_size + 8, 1);
  new_iheap->visitation_bit = 1;

  int padding = (unsigned long)new_iheap->memory_block % 8 == 0 ? 0 : 8 - (unsigned long)new_iheap->memory_block % 8;

  new_iheap->end_of_memory_block = new_iheap->memory_block + padding + No_pages * page_size;

  new_iheap->num_active_pages = 0;

  for (int i = 0; i < new_iheap->num_pages; i++)
  {
    void *start_of_page = new_iheap->memory_block + padding + No_pages * page_size * (i) / new_iheap->num_pages;
    new_iheap->page_arr[i] = page_init(page_size, start_of_page);
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
  free(h->memory_block);
  free(h);
}

unsigned int get_size_and_format_vector(bool *format_vector, char *format_string, int *len, bool get_format_vector)
{
  unsigned int bytes = 0;
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

    int padding = bytes % size == 0 ? 0 : size - bytes % size;

    if (get_format_vector && (bytes + padding) % 8 == 0)
    {
      format_vector[idx++] = *c == '*';
    }
    bytes += padding + size;
  }

  int padding = bytes % 8 == 0 ? 0 : 8 - bytes % 8;
  bytes += padding;

  if (get_format_vector)
  {
    *len = idx;
  }

  return bytes;
}

unsigned int get_size_of_struct(char *format_string)
{
  return get_size_and_format_vector(NULL, format_string, NULL, false);
}

bool heap_has_room(internal_heap_t *i_heap, unsigned int bytes)
{
  if (i_heap->num_active_pages < i_heap->num_pages / 2)
  {
    return true;
  }
  else
  {
    for (int i = 0; i < i_heap->num_pages; i++)
    {
      if (has_room(i_heap->page_arr[i], bytes))
      {
        return true;
      }
    }
    return false;
  }
}

page_t *get_alloc_page(internal_heap_t *h, unsigned int bytes)
{
  for (int i = 0; i < h->num_pages; i++)
  {
    if (has_room(h->page_arr[i], bytes))
    {
      return h->page_arr[i];
    }
  }

  assert(h->num_active_pages < h->num_pages/2);

  for (int i = 0; i < h->num_pages; i++)
  {
    if (!is_active(h->page_arr[i]))
    {
      make_active(h->page_arr[i]);
      h->num_active_pages++;

      assert(h->num_active_pages <= h->num_pages/2);

      return h->page_arr[i];
    }
  }

  return NULL;
}

void *h_alloc_struct_internal(internal_heap_t *h, char *format_string)
{
  bool format_vector[strlen(format_string)];
  int len = 0;

  unsigned int bytes = get_size_and_format_vector(format_vector, format_string, &len, true);

  assert(bytes + sizeof(void *) <= h->page_size);

  return page_alloc_struct(get_alloc_page(h, bytes), format_vector, len, bytes, h->visitation_bit);
}

void *h_alloc_data_internal(internal_heap_t *h, unsigned int bytes)
{
  int padding = bytes % 8 == 0 ? 0 : 8 - bytes % 8;
  bytes = bytes >= 8 ? bytes + padding : 8;

  assert(bytes + sizeof(void *) <= h->page_size);

  return page_alloc_data(get_alloc_page(h, bytes), bytes, h->visitation_bit);
}

bool is_valid_ptr(internal_heap_t *h, void *ptr)
{
  if (ptr < h->memory_block || ptr > h->end_of_memory_block)
  {
    return false;
  }

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

  return sizeof(internal_heap_t) + h->num_pages * sizeof(page_t *) + sum;
}

page_t **get_active_pages(internal_heap_t *h, int *len)
{
  page_t **active_pages = calloc(h->num_active_pages, sizeof(page_t *));

  int ind = 0;
  for (int i = 0; i < h->num_pages; i++)
  {
    page_t *curr_page = h->page_arr[i];
    if (is_active(curr_page))
    {
      active_pages[ind++] = curr_page;
    }
  }

  *len = ind;
  return active_pages;
}

page_t **get_passive_pages(internal_heap_t *h, int *len)
{
  page_t **passive_pages = calloc(h->num_pages - h->num_active_pages, sizeof(page_t *));

  int ind = 0;
  for (int i = 0; i < h->num_pages; i++)
  {
    page_t *curr_page = h->page_arr[i];
    if (!is_active(curr_page))
    {
      passive_pages[ind++] = curr_page;
    }
  }

  *len = ind;
  return passive_pages;
}

unsigned long avail_space(internal_heap_t *h)
{
  int max_pages = h->num_pages / 2;
  assert(max_pages >= h->num_active_pages);
  unsigned long avail_space = (max_pages - h->num_active_pages) * h->page_size;
  for (size_t i = 0; i < h->num_pages; i++)
  {
    if (is_active(h->page_arr[i]))
    {
      avail_space += avail_space_page(h->page_arr[i]);
    }
  }
  return avail_space;
}

unsigned long used_space(internal_heap_t *h)
{
  int max_pages = h->num_pages / 2;
  unsigned long used_space = 0;
  assert(h->num_active_pages <= max_pages);
  for (size_t i = 0; i < h->num_pages; i++)
  {
    if (is_active(h->page_arr[i]))
    {
      used_space += used_space_page(h->page_arr[i]);
    }
  }
  return used_space;
}