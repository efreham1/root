#include "hash_table.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "hash_table_internal.h"


static ht_entry_t *entry_create(elem_t key, elem_t value, ht_entry_t *next, heap_t *h)
{
    ht_entry_t *entry_p = h_alloc_struct(h, "***");
    ht_entry_t entry = {.key = key, .value = value, .next = next};
    *entry_p = entry;
    return entry_p;
}

static ht_entry_t *find_previous_entry_for_key(ioopm_hash_table_t *ht, ht_entry_t *sentinel, elem_t key)
{
    if (sentinel->next == NULL)
    {
        return sentinel; // bucket is empty
    }
    // else
    ht_entry_t *cursor = sentinel;
    ht_entry_t *next_entry = cursor->next;
    while (next_entry != NULL && ht->compare_lessthan_keys(next_entry->key, key))
    {
        cursor = next_entry;
        next_entry = cursor->next;
    }
    ht_entry_t *previous_entry = cursor;
    return previous_entry;
}

static ht_entry_t *get_sentinel(ioopm_hash_table_t *ht, elem_t key)
{
    /// Calculate the bucket for this entry
    int bucket = ht->h_fnc(key, ht->number_of_buckets);
    // get correct sentinel from buckets
    ht_entry_t *sentinel = &ht->buckets[bucket];
    return sentinel;
}

static ht_entry_t *get_sentinel_bucket(ioopm_hash_table_t *ht, int i)
{
    assert(i > -1 && i < ht->number_of_buckets);
    /// Calculate the bucket for this entry
    int bucket = i;
    // get correct sentinel from buckets
    ht_entry_t *sentinel = &ht->buckets[bucket];
    return sentinel;
}

static void set_NO_buckets(ioopm_hash_table_t *ht, heap_t *h)
{
    ht->number_of_buckets = ht->capacity/ht->load_factor > 19? 19: ht->capacity/ht->load_factor;
    
    char str[ht->number_of_buckets*2+2];
    sprintf(str, "%d*", 3*ht->number_of_buckets);

    ht->buckets = h_alloc_struct(h, str);
}

static void transfer_and_delete(ioopm_hash_table_t *from_ht, ioopm_hash_table_t *to_ht)
{   
    to_ht->buckets = from_ht->buckets;
    to_ht->number_of_buckets = from_ht->number_of_buckets;
    to_ht->NO_entries = from_ht->NO_entries;
    
}

static void update_NO_buckets(ioopm_hash_table_t *ht, heap_t *h)
{
    printf("\nupdating buckets\n");
    int new_capacity = ht->capacity*1.5;
    ht->capacity = new_capacity;
    ioopm_hash_table_t *new_ht = ioopm_hash_table_create_spec(ht->load_factor, new_capacity, ht->h_fnc, ht->compare_equal_keys, ht->compare_equal_values, ht->compare_lessthan_keys, h);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht, h);
    ioopm_list_t *values = ioopm_hash_table_values(ht, h);
    for (int i = 0; i < ioopm_linked_list_size(keys); i++)
    {
        ioopm_hash_table_insert(new_ht, ioopm_linked_list_get(keys, i), ioopm_linked_list_get(values, i), h);
    }
    transfer_and_delete(new_ht, ht);
}

// Create a new hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_function, ioopm_eq_function compare_eq_key, ioopm_eq_function compare_eq_values, ioopm_lt_function compare_lt_keys, heap_t *h)
{
    return ioopm_hash_table_create_spec(0.75, 200, hash_function, compare_eq_key, compare_eq_values, compare_lt_keys, h);
}

ioopm_hash_table_t *ioopm_hash_table_create_spec(float load_factor, int capacity, ioopm_hash_function hash_function, ioopm_eq_function compare_eq_key, ioopm_eq_function compare_eq_values, ioopm_lt_function compare_lt_keys, heap_t *h)
{
    ioopm_hash_table_t *hash_table = h_alloc_struct(h,"5*f3i");
    hash_table->h_fnc = hash_function;
    hash_table->compare_equal_keys = compare_eq_key;
    hash_table->compare_lessthan_keys = compare_lt_keys;
    hash_table->compare_equal_values = compare_eq_values;
    assert(load_factor>0);
    assert(capacity>=0);
    hash_table->load_factor = load_factor;
    hash_table->capacity = capacity;
    set_NO_buckets(hash_table, h);
    hash_table->NO_entries = 0;
    return hash_table;
}


// add key => value entry in hash table ht
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value, heap_t *h)
{
    if (ht->capacity >= ht->NO_entries && ht->number_of_buckets != 19)
    {
        update_NO_buckets(ht, h);
    }

    ht_entry_t *sentinel = get_sentinel(ht, key);
    ht_entry_t *prev_entry = find_previous_entry_for_key(ht, sentinel, key);
    ht_entry_t *curr_entry = prev_entry->next;

    /// Check if the next entry should be updated or not
    if (curr_entry != NULL && ht->compare_equal_keys(curr_entry->key, key))
    {
        curr_entry->value = value;
    }
    else
    {
        prev_entry->next = entry_create(key, value, curr_entry, h);
        ht->NO_entries++;
    }
}

// lookup value for key in hash table ht
elem_t *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key)
{
    ht_entry_t *sentinel = get_sentinel(ht, key);
    ht_entry_t *prev_entry = find_previous_entry_for_key(ht, sentinel, key);
    ht_entry_t *curr_entry = prev_entry->next;

    if (curr_entry != NULL && ht->compare_equal_keys(curr_entry->key, key))
    {
        return &curr_entry->value;
    }
    // else
    return NULL;
}

// remove any mapping from key to a value
elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key)
{
    ht_entry_t *sentinel = get_sentinel(ht, key);
    ht_entry_t *prev_entry = find_previous_entry_for_key(ht, sentinel, key);
    ht_entry_t *curr_entry = prev_entry->next;
    
    if (curr_entry != NULL && ht->compare_equal_keys(curr_entry->key, key)) // key found
    {
        prev_entry->next = curr_entry->next;
        elem_t data_ptr = curr_entry->value;
        ht->NO_entries--;
        return data_ptr;
    }
    else
    {
        assert(false);

    }
}



size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
    return ht->NO_entries;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
    return ht->NO_entries == 0;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
    for (int i = ht->number_of_buckets - 1; i >= 0; i--)
    {
        ht_entry_t *sentinel = get_sentinel_bucket(ht, i);
        sentinel->next = NULL;
    }
    ht->NO_entries = 0;
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht, heap_t *h)
{
    ioopm_list_t *list = ioopm_linked_list_create(ht->compare_equal_keys, h);
    for (int i = 0; i < ht->number_of_buckets; i++)
    {
        ht_entry_t *sentinel = get_sentinel_bucket(ht, i);
        ht_entry_t *next_entry = sentinel->next;
        while (next_entry != NULL)
        {
            printf("\nappend\n");
            ioopm_linked_list_append(list, next_entry->key, h);
            next_entry = next_entry->next;
        }
    }
    return list;
}

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht, heap_t *h)
{
    ioopm_list_t *list = ioopm_linked_list_create(ht->compare_equal_values, h);
    for (int i = 0; i < ht->number_of_buckets; i++)
    {
        ht_entry_t *sentinel = get_sentinel_bucket(ht, i);
        ht_entry_t *next_entry = sentinel->next;
        while (next_entry != NULL)
        {
            ioopm_linked_list_append(list, next_entry->value, h);
            next_entry = next_entry->next;
        }
    }
    return list;
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate_ht pred, void *arg)
{
    if (ioopm_hash_table_is_empty(ht))
    {
        return false;
    }

    for (int i = 0; i < ht->number_of_buckets; i++)
    {
        ht_entry_t *sentinel = get_sentinel_bucket(ht, i);
        ht_entry_t *next_entry = sentinel->next;
        while (next_entry != NULL)
        {
            elem_t current_key = next_entry->key;
            elem_t current_value = next_entry->value;
            if (!pred(current_key, current_value, arg))
            {
                return false;
            }
            next_entry = next_entry->next;
        }
    }
    return true;
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate_ht pred, void *arg)
{
    for (int i = 0; i < ht->number_of_buckets; i++)
    {
        ht_entry_t *sentinel = get_sentinel_bucket(ht, i);
        ht_entry_t *next_entry = sentinel->next;
        while (next_entry != NULL)
        {
            elem_t current_key = next_entry->key;
            elem_t current_value = next_entry->value;
            if (pred(current_key, current_value, arg))
            {
                return true;
            }
            next_entry = next_entry->next;
        }
    }
    return false;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function_ht apply_fun, void *arg)
{
    for (int i = 0; i < ht->number_of_buckets; i++)
    {
        ht_entry_t *sentinel = get_sentinel_bucket(ht, i);
        ht_entry_t *next_entry = sentinel->next;
        while (next_entry != NULL)
        {
            elem_t current_key = next_entry->key;
            elem_t *current_value_p = &next_entry->value;
            apply_fun(current_key, current_value_p, arg);
            next_entry = next_entry->next;
        }
    }
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
    for (int i = 0; i < ht->number_of_buckets; i++)
    {
        ht_entry_t *sentinel = get_sentinel_bucket(ht, i);
        ht_entry_t *next_entry = sentinel->next;
        while (next_entry != NULL)
        {
            elem_t current_key = next_entry->key;
            printf("\nentry: %p\nkey: %d\nvalue: %d\nptr: %p\n", next_entry, next_entry->key.int_v, next_entry->value.int_v, next_entry->next);
            if (ht->compare_equal_keys(current_key, key))
            {
                return true;
            }
            next_entry = next_entry->next;
        }
    }
    return false;
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
    for (int i = 0; i < ht->number_of_buckets; i++)
    {
        ht_entry_t *sentinel = get_sentinel_bucket(ht, i);
        ht_entry_t *next_entry = sentinel->next;
        while (next_entry != NULL)
        {
            elem_t current_value = next_entry->value;
            if (ht->compare_equal_values(current_value, value))
            {
                return true;
            }
            next_entry = next_entry->next;
        }
    }
    return false;
}


int ioopm_hash_table_number_of_buckets(ioopm_hash_table_t *ht)
{
    return ht->number_of_buckets;
}

void ioopm_hash_table_save_to_file(ioopm_hash_table_t *ht, FILE *f)
{
    fwrite(ht, sizeof(ioopm_hash_table_t), 1, f);
    fwrite(ht->buckets, sizeof(ht_entry_t), ht->number_of_buckets, f);
    for (int i = 0; i < ht->number_of_buckets; i++)
    {
        ht_entry_t *current_entry = ht->buckets[i].next;
        while (current_entry != NULL)
        {
            fwrite(current_entry, sizeof(ht_entry_t), 1, f);
            current_entry = current_entry->next;
        }
    }
}

ioopm_hash_table_t *ioopm_hash_table_load_from_file(heap_t *h, FILE *f, ioopm_hash_function hash_function, ioopm_eq_function compare_eq_key, ioopm_eq_function compare_eq_values, ioopm_lt_function compare_lt_keys)
{
    ioopm_hash_table_t *ht = h_alloc_struct(h, "5*f3i");
    fread(ht, sizeof(ioopm_hash_table_t), 1, f);
    char str[ht->number_of_buckets*2+2];
    sprintf(str, "%d*", 3*ht->number_of_buckets);
    ht->buckets = h_alloc_struct(h, str);
    fread(ht->buckets, sizeof(ht_entry_t), ht->number_of_buckets, f);
    for (int i = 0; i < ht->number_of_buckets; i++)
    {
        ht_entry_t *prev_entry = get_sentinel_bucket(ht, i);
        ht_entry_t *current_entry = ht->buckets[i].next;
        while (current_entry != NULL)
        {
            current_entry = h_alloc_struct(h, "3*");
            fread(current_entry, sizeof(ht_entry_t), 1, f);
            prev_entry->next = current_entry;
            prev_entry = current_entry;
            current_entry = current_entry->next;
        }
    }

    ht->h_fnc = hash_function;
    ht->compare_equal_keys = compare_eq_key;
    ht->compare_equal_values = compare_eq_values;
    ht->compare_lessthan_keys = compare_lt_keys;

    return ht;
}