#include "data_structure.h"

#pragma once

struct entry_ht
{
    elem_t key;       // holds the key
    elem_t value;     // holds the value
    ht_entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
    ht_entry_t *buckets;
    ioopm_hash_function h_fnc;
    ioopm_eq_function compare_equal_keys;
    ioopm_eq_function compare_equal_values;
    ioopm_lt_function compare_lessthan_keys;
    float load_factor;
    int capacity;
    int number_of_buckets;
    int NO_entries;
};