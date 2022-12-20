#include "data_structure.h"

#pragma once

struct list {
    ll_entry_t *head;
    ll_entry_t *tail;
    size_t size;
    ioopm_eq_function eq_fn;
};

struct entry_ll {
    elem_t value;
    ll_entry_t *next;
};