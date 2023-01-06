#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// For genericity:
typedef union elem elem_t;

typedef bool (*ioopm_eq_function) (elem_t a, elem_t b);
typedef bool (*ioopm_lt_function) (elem_t a, elem_t b);
typedef int (*ioopm_hash_function) (elem_t key, int buckets);

union elem {
    int int_v;
    unsigned int u_int_v;
    bool bool_v;
    float float_v;
    char char_v;
    void *ptr_v;
};

// For hash tables:
typedef struct hash_table ioopm_hash_table_t;
typedef struct entry_ht ht_entry_t;

typedef bool (*ioopm_predicate_ht) (elem_t key, elem_t value, void *extra);
typedef void (*ioopm_apply_function_ht) (elem_t key, elem_t *value, void *extra);

// For linked lists:
typedef struct list ioopm_list_t;
typedef struct entry_ll ll_entry_t;

typedef bool (*ioopm_predicate_ll) (elem_t value, void *extra);
typedef void (*ioopm_apply_function_ll) (elem_t *value, void *extra);
