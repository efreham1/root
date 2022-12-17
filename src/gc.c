#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "gc.h"
#include "heap.h"
#include "structs.h"

heap_t *h_init(unsigned long bytes, bool unsafe_stack, float gc_threshold)
{
  heap_t *newHeap = calloc(1,sizeof(heap_t));

  unsigned int page_size = 2048;

  unsigned int No_pages = (bytes/(page_size))*2 + 2;

  newHeap->internal_heap = h_init_internal(No_pages, page_size);
  newHeap->unsafe_stack = unsafe_stack;
  newHeap->gcTrigger = gc_threshold;

  return newHeap;
}

void h_delete(heap_t *h)
{
  h_delete_internal(h->internal_heap);
  free(h);
}

void ***get_valid_ptrs(internal_heap_t *i_heap, int *len)
{
  int stack_ptrs_len = 0;

  void ***stack_ptrs = NULL; //TODO insert stack function thing.

  void *buf[stack_ptrs_len];

  int idx = 0;
  for (size_t i = 0; i < stack_ptrs_len; i++)
  {
    if (is_valid_ptr(i_heap, stack_ptrs[i]))
    {
      buf[idx++] = stack_ptrs[i];
    }
  }
  *len = idx;

  void ***valid_ptrs = calloc(idx, sizeof(void **));

  for (size_t i = 0; i < idx; i++)
  {
    valid_ptrs[i] = buf[i]; 
  }
  //free(stack_ptrs);
  return valid_ptrs;
}

void h_delete_dbg(heap_t *h, void *dbg_value)
{
  int len = 0;
  void ***ptrs = get_valid_ptrs(h->internal_heap, &len);
  for (size_t i = 0; i < len; i++)
  {
    *ptrs[i] = NULL;
  }
  free(ptrs);
  h_delete(h);
}

int calc_buf_length(char *str)
{
  char *c = str;
  int sum = 0;
  int term = 0;
  while(*c != '\0')
  {
    if(isdigit(*c))
    {
      term = term*10 + ((int) *c -'0');
    }
    else
    {
      sum += term == 0? 1: term;
      term = 0;
    }

    c++;
  }
  return sum;
}

char *handle_input(char *str)
{
  char buf[calc_buf_length(str)];
  char *c = str;
  bool was_number_flag = false;
  int reps = 1;
  int idx = 0;
  while(*c!='\0')
  {
    if(strchr("*ilcdf",*c))
    {
      for (int i = 0; i < reps; i++)
      {
        buf[idx++] = *c;
      }
      
      reps = 1;
      was_number_flag = false;
    }
    else if (isdigit(*c))
    {
      if (was_number_flag)
      {
        reps = reps *10 + (int) (*c-'0');
      }
      else if (*c == '0')
      {
        return (char *) 0xDEADCAFEBABEBEEF;
      }
      else
      {
        reps = (int) (*c-'0');
        was_number_flag = true;
      }
      
    }
    else
    {
      return (char *) 0xDEADCAFEBABEBEEF;
    }
    c++;
  }
  buf[idx++] = '\0';

  if (buf[0] == '\0')
  {
    return (char *) 0xDEADCAFEBABEBEEF;
  }
  

  char *return_string = calloc(idx, 1);

  for (int i = 0; i < idx; i++)
  {
    return_string[i] = buf[i];
  }
  return return_string;
}

void *h_alloc_struct(heap_t *h, char *layout)
{
  char *format_string = handle_input(layout);
  void *ptr = h_alloc_struct_internal(h->internal_heap, format_string);
  free(format_string);
  return ptr;
}

void *h_alloc_data(heap_t *h, unsigned int bytes)
{
 return  h_alloc_data_internal(h->internal_heap, bytes);
}


unsigned int h_gc(heap_t *h)
{
  //TODO: STUB
  if(h){ puts("Not Null") ; } else { puts("NULL") ; }
  
  return 0;
}

size_t get_heap_actual_size(heap_t *h)
{
  return sizeof(heap_t) + get_internal_heap_actual_size(h->internal_heap);
}