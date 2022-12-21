#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "gc.h"
#include "heap.h"
#include "structs.h"
#include "stack_scanner.h"
#include "mover.h"

heap_t *h_init(unsigned long bytes, bool unsafe_stack, float gc_threshold)
{
  assert(gc_threshold > 0.0 && gc_threshold <= 1.0);
  heap_t *newHeap = calloc(1, sizeof(heap_t));

  unsigned int page_size = 2048;

  unsigned int No_pages = (bytes / page_size) * 2 + 2;

  newHeap->internal_heap = h_init_internal(No_pages, page_size);
  newHeap->unsafe_stack = unsafe_stack;
  newHeap->gcTrigger = gc_threshold;
  newHeap->tot_size = No_pages * page_size;

  return newHeap;
}

void h_delete(heap_t *h)
{
  h_delete_internal(h->internal_heap);
  free(h);
}

void ***get_valid_ptrs(internal_heap_t *i_heap, int *len, void ***proof_reading_arr)
{
  int stack_ptrs_len = 0;

  void ***stack_ptrs = stack_addresses(i_heap->end_of_memory_block, i_heap->memory_block, &stack_ptrs_len);

  void **buf1[stack_ptrs_len];
  void *buf2[stack_ptrs_len];

  int idx = 0;
  for (size_t i = 0; i < stack_ptrs_len; i++)
  {
    if (is_valid_ptr(i_heap, *stack_ptrs[i]))
    {
      buf1[idx] = stack_ptrs[i];
      buf2[idx++] = *stack_ptrs[i];
    }
  }
  *len = idx;

  void ***valid_ptrs = calloc(idx, sizeof(void **));

  *proof_reading_arr = calloc(idx, sizeof(void *));

  for (size_t i = 0; i < idx; i++)
  {
    valid_ptrs[i] = buf1[i];
    (*proof_reading_arr)[i] = buf2[i];
  }
  free(stack_ptrs);
  return valid_ptrs;
}

void h_delete_dbg(heap_t *h, void *dbg_value)
{
  int len = 0;
  void **proof_reading_arr = NULL;
  void ***ptrs = get_valid_ptrs(h->internal_heap, &len, &proof_reading_arr);
  for (size_t i = 0; i < len; i++)
  {
    if (proof_reading_arr[i] == *ptrs[i])
    {
      *ptrs[i] = NULL;
    }
  }
  free(ptrs);
  free(proof_reading_arr);
  h_delete(h);
}

int calc_buf_length(char *str)
{
  char *c = str;
  int sum = 0;
  int term = 0;
  while (*c != '\0')
  {
    if (isdigit(*c))
    {
      term = term * 10 + ((int)*c - '0');
    }
    else
    {
      sum += term == 0 ? 1 : term;
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
  while (*c != '\0')
  {
    if (strchr("*ilcdf", *c))
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
        reps = reps * 10 + (int)(*c - '0');
      }
      else if (*c == '0')
      {
        return (char *)0xDEADCAFEBABEBEEF;
      }
      else
      {
        reps = (int)(*c - '0');
        was_number_flag = true;
      }
    }
    else
    {
      return (char *)0xDEADCAFEBABEBEEF;
    }
    c++;
  }
  buf[idx++] = '\0';

  if (buf[0] == '\0')
  {
    return (char *)0xDEADCAFEBABEBEEF;
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

  if (avail_space(h->internal_heap) - h->tot_size / 2 * (1 - h->gcTrigger) < strlen(format_string) * 8 + sizeof(void *))
  {
    assert(h_gc(h) != 0 || avail_space(h->internal_heap) > strlen(format_string) * 8 + sizeof(void *));
  }

  void *ptr = h_alloc_struct_internal(h->internal_heap, format_string);
  free(format_string);
  return ptr;
}

void *h_alloc_data(heap_t *h, unsigned int bytes)
{
  if ((float)((used_space(h->internal_heap) + bytes + sizeof(void *))*2)/h->tot_size >= h->gcTrigger)
  {
    assert(h_gc(h) != 0 || has_room(h->internal_heap) > bytes + sizeof(void *));
  }

  return h_alloc_data_internal(h->internal_heap, bytes);
}

unsigned int h_gc(heap_t *h)
{
  unsigned int used_prior = h_used(h);

  int len = 0;
  void **proof_reading_arr = NULL;
  void ***ptrs = get_valid_ptrs(h->internal_heap, &len, &proof_reading_arr);

  move(h->internal_heap, ptrs, len, h->unsafe_stack, proof_reading_arr);

  free(ptrs);
  free(proof_reading_arr);
  printf("\nSKRKPSAMLING!!! cleaned: %d\n", used_prior - h_used(h));
  return used_prior - h_used(h);
}

unsigned int h_gc_dbg(heap_t *h, bool unsafe_stack)
{
  unsigned int used_prior = h_used(h);

  int len = 0;
  void **proof_reading_arr = NULL;
  void ***ptrs = get_valid_ptrs(h->internal_heap, &len, &proof_reading_arr);

  move(h->internal_heap, ptrs, len, unsafe_stack, proof_reading_arr);

  free(ptrs);
  free(proof_reading_arr);
  printf("\nSKRKPSAMLING!!! cleaned: %d\n", used_prior - h_used(h));
  return used_prior - h_used(h);
}

size_t get_heap_actual_size(heap_t *h)
{
  return sizeof(heap_t) + get_internal_heap_actual_size(h->internal_heap);
}

unsigned int h_avail(heap_t *h)
{
  return avail_space(h->internal_heap);
}

unsigned int h_used(heap_t *h)
{
  return used_space(h->internal_heap);
}
