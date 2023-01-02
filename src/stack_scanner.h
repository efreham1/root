/// \file

#include "heap.h"
#include "page.h"
#include "gc.h"

#ifndef __stack__
#define __stack__

/// @brief scanns the stack for addresses that might be pointers to the heap
/// @param top_valid_address the stack top adress
/// @param bottom_valid_address the stack bottom adress
/// @param len the number of adresses gets saved to this adress
void ***stack_addresses(void *top_valid_address, void *bottom_valid_address, int *len);

#endif