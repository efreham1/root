#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "gc.h"
#include "heap.h"

struct external_heap
{
  internal_heap_t *heapPtr;
  bool unsafe_stack;  
  float gcTrigger;
  //  void **_arr (allokeringsmap?) BITVEKTOR!
};

heap_t *h_init(unsigned int No_pages, bool unsafe_stack, float gc_threshold)
{
  assert(No_pages >= 2);
  heap_t *newHeap = calloc(1,sizeof(heap_t));
  newHeap->heapPtr = h_init_internal(No_pages, 2048);
  newHeap->unsafe_stack = unsafe_stack;
  newHeap->gcTrigger = gc_threshold;

  return newHeap;
}

void h_delete(heap_t *h)
{
  h_delete_internal(h->heapPtr);
  free(h);
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
  void *ptr = h_alloc_struct_internal(h->heapPtr, format_string);
  free(format_string);
  return ptr;
}

void *h_alloc_data(heap_t *h, unsigned int bytes)
{
 return  h_alloc_data_internal(h->heapPtr, bytes);
}
