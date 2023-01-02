/// \file

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#ifndef __page__
#define __page__

typedef struct page page_t;
/// @brief gets the size of a page
/// @param p the page
/// @return the page size
size_t get_page_actual_size(page_t *p);

/// @brief creates a page
/// @param bytes size of a page
/// @param page_start 
/// @return a page
page_t *page_init(unsigned int bytes, void *page_start);

/// @brief delete a page
/// @param p the page to delete
void page_delete(page_t *p);

/// @brief checks if a page is active
/// @param p the page to check
/// @return true if page is active
bool is_active(page_t *p);

/// @brief checks if a page has room for x bytes
/// @param p the page
/// @param bytes the bytes
/// @return true if a page has space
bool has_room(page_t *p, unsigned int bytes);

/// @brief allocates data on a page
/// @param page the page
/// @param bytes the size of the data
void *page_alloc_data(page_t *page, unsigned int bytes);

/// @brief allocates a struct on the page
/// @param page the page
/// @param format_vector the string specifying what the struct contains 
/// @param len 
/// @param bytes size of the struct
void *page_alloc_struct(page_t *page, bool *format_vector, int len, unsigned int bytes);

/// @brief makes a page active
/// @param p the page
void make_active(page_t *p);

/// @brief makes a page passive
/// @param p the page
void make_passive(page_t *p);

/// @brief checks if a pointer is pointing to a page
/// @param p the page 
/// @param ptr the pointer
/// @return true if it points to the page
bool is_ptr_to_page(page_t *p, void *ptr);

/// @brief checks the available space on a page
/// @param p the page
/// @return the amount of available space
unsigned int avail_space_page(page_t *p);

/// @brief check the used space on a page
/// @param p the pag
/// @return the amount of used space
unsigned int used_space_page(page_t *p);

#endif