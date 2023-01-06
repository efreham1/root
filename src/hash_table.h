#include "data_structure.h"
#include "linked_list.h"
#include "gc.h"

#pragma once

/**
 * @file hash_table.h
 * @author Fredrik Hammarberg, Ville Elings
 * @date 22-09-13
 * @brief Simple hash table that maps integer keys to string values.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 *
 * @see https://uppsala.instructure.com/courses/68435/assignments/130155
 */

/// @param hash_function the hash function that will hash the keys
/// @param compare_eq_keys an equals function that will compare the keys
/// @param compare_eq_values an equals function that will compare the values
/// @param compare_lt_keys a less than function that will compare the keys
/// @brief Create a new hash table
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_function, ioopm_eq_function compare_eq_key, ioopm_eq_function compare_eq_values, ioopm_lt_function compare_lt_keys, heap_t *h);

/// @param load_factor how full the hash table is allowed to be
/// @param capacity how many elements the hash table initially can hold
/// @param hash_function the hash function that will hash the keys
/// @param compare_eq_keys an equals function that will compare the keys
/// @param compare_eq_values an equals function that will compare the values
/// @param compare_lt_keys a less than function that will compare the keys
/// @brief Create a new hash table
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create_spec(float load_factor, int capacity, ioopm_hash_function hash_function, ioopm_eq_function compare_eq_key, ioopm_eq_function compare_eq_values, ioopm_lt_function compare_lt_keys, heap_t *h);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value, heap_t *h);

/// @brief lookup adress of value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @return the adress of the value mapped from the key, address is NULL if key doesn't exist
elem_t *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return the value previously mapped from the key
elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key);

/// @brief returns the number of key => value entries in the hash table
/// @param ht hash table operated upon
/// @return the number of key => value entries in the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

/// @brief checks if the hash table is empty
/// @param ht hash table operated upon
/// @return true is size == 0, else false
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

/// @brief clear all the entries in a hash table
/// @param ht hash table operated upon
void ioopm_hash_table_clear(ioopm_hash_table_t *ht);

/// @brief return the keys for all entries in a hash table (in no particular order, but same as ioopm_hash_table_values)
/// @param ht hash table operated upon
/// @return a linked list of the keys for hash table ht
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht, heap_t *h);

/// @brief return the values for all entries in a hash table (in no particular order, but same as ioopm_hash_table_keys)
/// @param ht hash table operated upon
/// @return a linked list of values for hash table ht
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht, heap_t *h);

/// @brief check if a hash table has an entry with a given key
/// @param ht hash table operated upon
/// @param key the key sought
/// @return true if the key exists otherwise false
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/// @brief check if a hash table has an entry with a given value
/// @param ht hash table operated upon
/// @param value the value sought
/// @return true if the value exists otherwise false
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value);

/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param ht hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @return true if the predicate holds for all entries otherwise false
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate_ht pred, void *arg);

/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param ht hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @return true if the predicate holds for any entry otherwise false
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate_ht pred, void *arg);

/// @brief apply a function to all entries in a hash table
/// @param ht hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function_ht apply_fun, void *arg);


/// @brief get the amount of buckets in the hash table
/// @param ht the hash table
/// @return the number of buckets in the hash table
int ioopm_hash_table_number_of_buckets(ioopm_hash_table_t *ht);

/// @brief save the hashtable on a binary file
/// @param ht the hash table to be saved
/// @param f an opened binary file to save the has table to
void ioopm_hash_table_save_to_file(ioopm_hash_table_t *ht, FILE *f);

/// @brief load a hash table from a file
/// @param f an opened binary file which the hash table is stored on
/// @param hash_function the hash function that will hash the keys
/// @param compare_eq_keys an equals function that will compare the keys
/// @param compare_eq_values an equals function that will compare the values
/// @param compare_lt_keys a less than function that will compare the keys
/// @return the created hash table
ioopm_hash_table_t *ioopm_hash_table_load_from_file(heap_t *h, FILE *f, ioopm_hash_function hash_function, ioopm_eq_function compare_eq_key, ioopm_eq_function compare_eq_values, ioopm_lt_function compare_lt_keys);