/// \file

#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include "page.h"

#ifndef __heap__
#define __heap__

typedef struct internal_heap internal_heap_t;

/// @brief created a internal heap
/// @param No_pages number of pages
/// @param page_size size of a page
/// @return returns a internal heap
internal_heap_t *h_init_internal(unsigned int No_pages, unsigned int page_size);

/// @brief deletes a internal heap
/// @param h the heap to be deleted
void h_delete_internal(internal_heap_t *h);

/// @brief allocates space of a struct on the internal heap
/// @param h the heap to allocate space on
/// @param layout what to allocate
void *h_alloc_struct_internal(internal_heap_t *h, char *layout);

/// @brief allocates space for data on the internal heap
/// @param h the thap to allocate space on
/// @param bytes how many bytes to allocate
void *h_alloc_data_internal(internal_heap_t *h, unsigned int bytes);

/// @brief function to check if a pointer is pointing to a heap
/// @param h the eap
/// @param ptr poiner to check
/// @return true if it points to the heap otherwise false
bool is_valid_ptr(internal_heap_t *h, void *ptr);

/// @brief get the size of a heap
/// @param h the heap to check
/// @return the size
size_t get_internal_heap_actual_size(internal_heap_t *h);

/// @brief finds all the active pages
/// @param h the heap where the pages exists
/// @param len address of where the number of active pages gets written to
/// @return a array of active pages
page_t **get_passive_pages(internal_heap_t *h, int *len);

/// @brief finds all the passive pages
/// @param h the heap where the passive pages exists
/// @param len address of where the number of passive pages gets written to
/// @return a array of passive pages
page_t **get_active_pages(internal_heap_t *h, int *len);

/// @brief checks how much sapce is availible on the heap
/// @param h the heap
/// @return the amount of space left on the heap
unsigned long avail_space(internal_heap_t *h);

/// @brief checks how much space is used on the heap
/// @param h the heap
/// @return the amount of space used
unsigned long used_space(internal_heap_t *h);

/// @brief gets the size of a struct
/// @param format_string the string specifying what the struct contains
/// @return the size of the struct
unsigned int get_size_of_struct(char *format_string);

/// @brief checks if a heap has room for x bytes
/// @param i_heap the heap 
/// @param bytes the number of bytes
/// @return true if it has space
bool heap_has_room(internal_heap_t *i_heap, unsigned int bytes);

#endif
