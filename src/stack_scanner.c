#include <string.h>
#include <stdlib.h>
#include "stack_scanner.h"

extern char **environ; // ADDRESS TO BOTTOM OF STACK

void ***stack_addresses (void *top_valid_address, void *bottom_valid_address, int *len)
{
  void *bottom_address = (void *) environ;
  void *top_address = __builtin_frame_address (0);

  int idx = 0;
  unsigned long nr_addresses = (bottom_address - top_address);

  void **buf[nr_addresses];

  memset (buf, 0, nr_addresses);

  unsigned long address_num = (unsigned long) top_address;

  // FILTER ADDRESSES
  for (int i = 0; i < nr_addresses - sizeof (void *); ++i)
    {
      void **ptr_to_ptr = (void **) (address_num + i);

      if (*ptr_to_ptr != NULL)
        {
          if ((unsigned long) *ptr_to_ptr % 4 == 0)
            {
              if (top_valid_address >= *ptr_to_ptr && *ptr_to_ptr >= bottom_valid_address)
                {
                  buf[idx++] = ptr_to_ptr;
                }
            }
        }
    }

  *len = idx;

  void ***stack_ptrs = calloc (idx, sizeof (void **));

  for (size_t i = 0; i < idx; i++)
    {
      stack_ptrs[i] = buf[i];
    }

  return stack_ptrs;
}