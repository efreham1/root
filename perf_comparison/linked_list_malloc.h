#pragma once
#include <stdbool.h>
#include "data_structure.h"

/*
 * @file hash_table.h
 * @author Joel Holmberg, Viktor Lundin
 * @date 4 Oct 2022
 * @brief Simple linked list that stores values.
 *
 * Header file provides definitions and documentation for all the functions that 
 * creates/operates on/destroys a linked list intended for using the interface.
 *
 * @see https://uppsala.instructure.com/courses/68435/assignments/130155
 */


/// @brief Creates a new empty list
/// @param eq_fn pointer to function that check values equality
/// @return an empty linked list
ioopm_list_t *ioopm_linked_list_malloc_create(ioopm_eq_function eq_fn);

/// @brief Tear down the linked list and return all its memory
/// @param list the list to be destroyed
void ioopm_linked_list_malloc_destroy(ioopm_list_t *list);

/// @brief Insert at the end of a linked list in O(1) time
/// @param list the linked list that will be appended
/// @param value the value to be appended
void ioopm_linked_list_malloc_append(ioopm_list_t *list, elem_t value);

/// @brief Insert at the front of a linked list in O(1) time
/// @param list the linked list that will be prepended to
/// @param value the value to be prepended
void ioopm_linked_list_malloc_prepend(ioopm_list_t *list, elem_t value);

/// @brief Insert an element into a linked list in O(n) time.
/// The valid values of index are [0,n] for a list of n elements,
/// where 0 means before the first element and n means after
/// the last element.
/// Invalid indexes return without changes.
/// Precondition: value != -1.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be inserted
void ioopm_linked_list_malloc_insert(ioopm_list_t *list, int index, elem_t value);

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// Invalid indexes return -1.
/// @param list the linked list
/// @param index the position in the list
/// @return the value removed
elem_t ioopm_linked_list_malloc_remove(ioopm_list_t *list, int index);

/// @brief Retrieve an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// Invalid indexes return -1.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @return the value at the given position
elem_t ioopm_linked_list_malloc_get(ioopm_list_t *list, int index);

/// @brief Test if an element is in the list
/// @param list the linked list
/// @param element the element sought
/// @return true if element is in the list, else false
bool ioopm_linked_list_malloc_contains(ioopm_list_t *list, elem_t element);

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list the linked list
/// @return the number of elements in the list
size_t ioopm_linked_list_malloc_size(ioopm_list_t *list);

/// @brief Test whether a list is empty or not
/// @param list the linked list
/// @return true if the number of elements in the list is 0, else false
bool ioopm_linked_list_malloc_is_empty(ioopm_list_t *list);

/// @brief Remove all elements from a linked list
/// @param list the linked list
void ioopm_linked_list_malloc_clear(ioopm_list_t *list);

/// @brief Test if a supplied property holds for all elements in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested (function pointer)
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for all elements in the list, else false
bool ioopm_linked_list_malloc_all(ioopm_list_t *list, ioopm_predicate_ll prop, void *extra);

/// @brief Test if a supplied property holds for any element in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for any elements in the list, else false
bool ioopm_linked_list_malloc_any(ioopm_list_t *list, ioopm_predicate_ll prop, void *extra);

/// @brief Apply a supplied function to all elements in a list.
/// @param list the linked list
/// @param fun the function to be applied
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of fun
void ioopm_linked_list_malloc_apply_to_all(ioopm_list_t *list, ioopm_apply_function_ll fun, void *extra);

/// @brief gets the first index for an element in a linked list
/// @param ll the linked list
/// @param element the element
/// @return the first index of the element, if the element wasn't found returns -1
int ioopm_linked_list_malloc_get_index(ioopm_list_t *ll, elem_t element);

/// @brief creates a copy of a given linked list
/// @param old_ll linked list to copy
/// @return a copy of old_ll
ioopm_list_t *ioopm_linked_list_malloc_copy(ioopm_list_t *old_ll);

/// @brief save a linked list to a binary file
/// @param ll the linked list to be saved
/// @param f an binary opened file to write the list to
void ioopm_linked_list_malloc_save_to_file(ioopm_list_t *ll, FILE *f);

/// @brief load a linked list from a binary file
/// @param f an opened binary file to read the list from
/// @param eq_fn pointer to function that check values equality
/// @return the loaded linked list
ioopm_list_t *ioopm_linked_list_malloc_load_from_file(FILE *f, ioopm_eq_function eq_fn);