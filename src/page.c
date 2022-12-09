#include <stdbool.h>
#include <stddef.h>

struct page
{
  bool isActive;
  size_t size;
  size_t offset;
  void *memoryBlock;
};

//int *adress = memoryBlock + offset;

