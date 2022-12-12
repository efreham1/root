#include "page.h"
#include "metadata.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

struct page
{
  bool is_active;
  unsigned int size;
  unsigned int offset;
  void * memory_block;
};

size_t get_page_struct_size()
{
  return sizeof(page_t) + 8;
}

page_t *page_init(unsigned int bytes)
{
  assert(bytes >= 16);
  page_t *newpage = calloc(1,sizeof(page_t));
  newpage->is_active = false;
  void *mem_block = calloc(bytes+8, 1);
  int padding = (unsigned long)mem_block%8 == 0? 0: 8 - (unsigned long)mem_block%8;
  newpage->memory_block = mem_block;
  newpage->offset = padding;
  newpage->size = bytes + padding;
  return newpage;
}

void page_delete(page_t *p)
{
  free(p->memory_block);
  free(p);
}

bool is_active(page_t *p) { return p->is_active ; }

void make_active(page_t *p) { p->is_active = true ; }

bool has_room(page_t *p, unsigned int bytes)
{
  if (!is_active(p))
  {
    return false;
  }
  
  

  int padding = bytes%8 == 0? 0: 8 - bytes%8;
  bytes = bytes >= 8? bytes + padding: 8;

  unsigned int needed_size = bytes + sizeof(metadata_t);

  return(p->size - p->offset >= needed_size);
}

void *page_alloc_data(page_t *page, unsigned int bytes)
{  
  metadata_t *metadata = (metadata_t *) (page->memory_block + page->offset);

  int padding = bytes%8 == 0? 0: 8 - bytes%8;
  bytes = bytes >= 8? bytes + padding: 8;

  *metadata = set_data_size((unsigned int) bytes);
  page->offset += sizeof(metadata_t);

  void *pointer = page->memory_block + page->offset;
  page->offset += bytes;
  return pointer;
}

void *page_alloc_struct(page_t *page, char *format_string)
{
  metadata_t *metadata = (metadata_t *) (page->memory_block + page->offset);
  page->offset += sizeof(metadata_t);

  unsigned int bytes = 0;
  int largest = 8;
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

    if (size>largest) largest = size;
    

    unsigned long intended_address = (unsigned long) page->memory_block + page->offset + bytes;

    int padding = intended_address%size == 0? 0: size - intended_address%size;

    if ((bytes+padding)%8 == 0)
    {
      format_vector[idx++] = *c == '*';
    }

    bytes += padding + size;
  }

  int padding = bytes%largest==0? 0: largest - bytes%largest;
  bytes += padding;
  printf("Total size: %d\nFormat string: %s\nFormat vector\n", bytes, format_string);
  
  for (size_t i = 0; i < idx; i++)
  {
    printf("%d\n", format_vector[i]);
  }

  *metadata = set_format_vector(format_vector, idx);

  void *pointer = page->memory_block + page->offset;
  page->offset += bytes;
  return pointer;
}