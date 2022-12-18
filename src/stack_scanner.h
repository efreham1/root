/// \file

#include "heap.h"
#include "page.h"
#include "gc.h"

#ifndef __stack__
#define __stack__

void ***stack_addresses(void *top_valid_address, void *bottom_valid_address, int *len);

#endif