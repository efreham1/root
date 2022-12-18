#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stack_scanner.h"

extern char **environ; // ADDRESS TO BOTTOM OF STACK

void ***stack_addresses(void *top_valid_address, void *bottom_valid_address, int *len)
{
    void *bottom_address = environ;
    void *top_address = __builtin_frame_address(0);
    int idx = 0;
    unsigned long nr_addresses = (bottom_address - top_address);

    void **buf[nr_addresses];

    /** FETCH POSSIBLE ADDRESSES IN STACK
     *  For a 64-bit system, addresses are of size 8 bytes.
     *  Jumping 8 bytes in memory will therefore return us possible addresses.
     *
     *  UPDATE:
     *  Perhaps jump by 1 byte to be compatible with all systems.
     *
     */

    char *bytes = (char *)top_address;

    for (size_t i = 0; i < nr_addresses - sizeof(void *); i++)
    {
        buf[idx++] = (void **)(bytes + i);
    }

    //------------------------------------------------------------------------------------------------------------------

    idx = 0;

    // FILTER ADDRESSES
    for (int i = 0; i < nr_addresses; ++i)
    {

        if (buf[i] != NULL)
        {
            // CAST HEXADECIMAL DATA TO DECIMAL
            unsigned long temp = (unsigned long)*buf[i];

            /** VERIFYING THAT GIVEN DATA IS AN ADDRESS
             *
             *  (ALPHA)
             *  Knowing that addresses are stored in increments of 8 bytes,
             *  for a 64-bit system, the bit representation of such addresses should be
             *  terminated with two 0-valued bits. The addresses are in other words divisible by 4,
             *  hence: address % 4 == 0
             *
             *  (DELTA)
             *  An intresting address should also point to a given piece of memory.
             *  By verifying that the address is within the span of this memory, such addresses may be included.
             *
             */
            if (temp % 4 == 0) // ALPHA
            {
                // WE ARE NOW GATHERING EVERYTHING IN THE STACK!!!
                if (top_valid_address >= *(buf[i]) && *(buf[i]) >= bottom_valid_address) // DELTA
                {
                    buf[idx++] = buf[i];
                }
            }
        }
    }

    *len = idx;

    void ***stack_ptrs = calloc(idx, sizeof(void **));

    for (size_t i = 0; i < idx; i++)
    {
        stack_ptrs[i] = buf[i];
    }
    return stack_ptrs;
}