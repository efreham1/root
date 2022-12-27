#include "inventory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "general_TUI.h"
#include "hash_table_internal.h"
#include "linked_list_internal.h"

static bool string_compare_function(elem_t e1, elem_t e2)
{
    char *str1 = ((char *)e1.ptr_v);
    char *str2 = ((char *)e2.ptr_v);

    printf("\nstr1: %s str2: %s\n", str1, str2);

    return strcmp(str1, str2) == 0;
}

static inventory_merch_t *create_merchandise(char *name, char *desc, int price, heap_t *h)
{
    inventory_merch_t *merch = h_alloc_struct(h, "**iii*");
    merch->name = name;
    merch->desc = desc;
    merch->price = price;
    merch->total_stock = 0;
    merch->storage_locations = ioopm_linked_list_create(string_compare_function, h);
    return merch;
}

void ioopm_inventory_add_merchandise(ioopm_inventory_t *inventory, char *name, char *desc, int price, heap_t *h)
{
    printf("\nname: %s desc: %s price: %d\n", name, desc, price);
    inventory_merch_t *merch = create_merchandise(name, desc, price, h);

    elem_t key = {.ptr_v = name};
    elem_t value = {.ptr_v = merch};
    ioopm_hash_table_insert(inventory->warehouse, key, value, h);
}

ioopm_list_t *ioopm_inventory_get_merchandise_list(ioopm_inventory_t *inventory, heap_t *h)
{
    return ioopm_hash_table_keys(inventory->warehouse, h);
}


void ioopm_inventory_remove_merchandise(ioopm_inventory_t *inventory, char *merch_name)
{
    elem_t key = {.ptr_v = merch_name};
    ioopm_hash_table_remove(inventory->warehouse, key);
}

void ioopm_inventory_edit_merchandise(ioopm_inventory_t *inventory, char *merch_name, char *new_name, char *new_desc, int new_price, heap_t *h)
{
    inventory_merch_t *merch = ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = merch_name})->ptr_v;
    if (new_name || new_desc || new_price != 0)
    {
        char *key = merch_name;

        char *name = NULL;
        if (new_name == NULL)
        {
            int len = strlen(merch_name) + 1;
            name = h_alloc_data(h, len);
            memcpy(name, key, len);
        }
        else
        {
            name = new_name;
        }

        char *desc = NULL;
        if (new_desc == NULL)
        {
            int len = strlen(merch->desc) + 1;
            desc = h_alloc_data(h, len);
            memcpy(desc, merch->desc, len);
        }
        else
        {
            desc = new_desc;
        }

        int price = new_price != 0 ? new_price : merch->price;
        int total_stock = merch->total_stock;
        ioopm_list_t *storage_locations = merch->storage_locations;

        inventory_merch_t *updated_merch = create_merchandise(name, desc, price, h);
        updated_merch->total_stock = total_stock;
        updated_merch->theoretical_stock = total_stock;
        updated_merch->storage_locations = storage_locations;

        ioopm_inventory_remove_merchandise(inventory, key);
        ioopm_hash_table_insert(inventory->warehouse, (elem_t){.ptr_v = name}, (elem_t){.ptr_v = updated_merch}, h);
    }
    else
    {
        return;
    }
}

void ioopm_inventory_replenish_existing_shelf_stock(ioopm_inventory_t *inventory, char *merch_name, int quantity, char *shelf, heap_t *h)
{
    inventory_merch_t *merch = ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = merch_name})->ptr_v;
    ioopm_list_t *storage_locations = merch->storage_locations;
    ioopm_list_iterator_t *iterator = ioopm_list_iterator(storage_locations, h);

    while ((strcmp(((char *)((storage_location_t *)(ioopm_iterator_current(iterator).ptr_v))->shelf), shelf) != 0))
    {
        ioopm_iterator_next(iterator);
    }

    ((storage_location_t *)ioopm_iterator_current(iterator).ptr_v)->stock += quantity;
    merch->total_stock += quantity;
    merch->theoretical_stock += quantity;
}

void ioopm_inventory_replenish_new_shelf_stock(ioopm_inventory_t *inventory, char *merch_name, int quantity, char *shelf, heap_t *h)
{
    inventory_merch_t *merch = ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = merch_name})->ptr_v;
    ioopm_list_t *storage_locations = merch->storage_locations;

    storage_location_t *new_shelf = h_alloc_struct(h, "*i");
    int len = strlen(shelf) + 1;
    char *shelf_cpy = h_alloc_data(h, len);
    memcpy(shelf_cpy, shelf, len);
    new_shelf->shelf = shelf_cpy;
    new_shelf->stock = quantity;

    ioopm_linked_list_append(storage_locations, (elem_t){.ptr_v = new_shelf}, h);
    
    ioopm_linked_list_append(inventory->used_shelves, (elem_t){.ptr_v = shelf_cpy}, h);
    merch->total_stock += quantity;
    merch->theoretical_stock += quantity;
}

ioopm_list_t *ioopm_inventory_storage_locations_merch(ioopm_inventory_t *inventory, char *merch_name)
{
    return ((inventory_merch_t *)ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = merch_name})->ptr_v)->storage_locations;
}

int ioopm_inventory_get_stock(ioopm_inventory_t *inventory, char *merch_name)
{
    return ((inventory_merch_t *)ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = merch_name})->ptr_v)->total_stock;
}

static int string_sum_hash(elem_t e, int buckets)
{
    char *name = ((char *)e.ptr_v);

    assert(name != NULL);
    
    int result = 0;
    while (*name != '\0')
    {
        result += *name++;
    }
    return abs(result % buckets);
}

static bool merch_compare_function(elem_t e1, elem_t e2)
{
    char *str1 = (char *)((inventory_merch_t *)e1.ptr_v)->name;
    char *str2 = (char *)((inventory_merch_t *)e2.ptr_v)->name;

    return strcmp(str1, str2) == 0;
}

static bool string_lt(elem_t e1, elem_t e2)
{
    return (strcmp(e1.ptr_v, e2.ptr_v) < 0);
}

static int get_password(void)
{
    FILE *f = fopen("demo/inlupp2/password.txt", "r");

    char *buf = NULL;
    size_t len = 0;
    getline(&buf, &len, f);
    assert(is_number(buf));

    fclose(f);

    int password = atoi(buf);
    free(buf);
    return password;
}

static int get_str_len_file(FILE *f)
{
    int len = 0;
    char buf = '0';
    while (buf != '\0')
    {
        fread(&buf, sizeof(char), 1, f);
        len++;
    }
    fseek(f, -len * sizeof(char), SEEK_CUR);
    return len;
}

static void save_storage_locs_to_file(ioopm_list_t *storage_locations, FILE *f)
{
    ioopm_linked_list_save_to_file(storage_locations, f);
    ll_entry_t *curr_entry = storage_locations->head;
    while (curr_entry)
    {
        fwrite(curr_entry->value.ptr_v, sizeof(storage_location_t), 1, f);
        char *shelf = ((storage_location_t *)curr_entry->value.ptr_v)->shelf;
        fwrite(shelf, sizeof(char), strlen(shelf) + 1, f);
        curr_entry = curr_entry->next;
    }
}

static ioopm_list_t *load_storage_locs_from_file(FILE *f, heap_t *h)
{
    ioopm_list_t *list = ioopm_linked_list_load_from_file(f, string_compare_function, h);
    ll_entry_t *curr_entry = list->head;
    while (curr_entry)
    {
        storage_location_t *sto_loc = h_alloc_struct(h, "*i");
        fread(sto_loc, sizeof(storage_location_t), 1, f);
        curr_entry->value.ptr_v = sto_loc;
        int str_len = get_str_len_file(f);
        char *shelf = h_alloc_data(h, str_len +1); 
        fread(shelf, sizeof(char), str_len, f);
        ((storage_location_t *)curr_entry->value.ptr_v)->shelf = shelf;
        curr_entry = curr_entry->next;
    }
    return list;
}

static void save_merch_to_file(inventory_merch_t *merch, FILE *f)
{
    fwrite(merch, sizeof(inventory_merch_t), 1, f);
    fwrite(merch->name, sizeof(char), strlen(merch->name) + 1, f);
    fwrite(merch->desc, sizeof(char), strlen(merch->desc) + 1, f);
    save_storage_locs_to_file(merch->storage_locations, f);
}

inventory_merch_t *load_merch_from_file(FILE *f, heap_t *h)
{
    inventory_merch_t *merch = h_alloc_struct(h, "**iii*"); 
    fread(merch, sizeof(inventory_merch_t), 1, f);
    int str_len = get_str_len_file(f);
    char *name = h_alloc_data(h, str_len +1); 
    fread(name, sizeof(char), str_len, f);
    merch->name = name;
    int str_len2 = get_str_len_file(f);
    char *desc = h_alloc_data(h, str_len2 +1);
    fread(desc, sizeof(char), str_len2, f);
    merch->desc = desc;
    merch->storage_locations = load_storage_locs_from_file(f, h);
    return merch;
}

void ioopm_inventory_save(ioopm_inventory_t *inventory, char *file_name)
{
    FILE *f = fopen(file_name, "wb");

    ioopm_hash_table_save_to_file(inventory->warehouse, f);

    for (int i = 0; i < inventory->warehouse->number_of_buckets; i++)
    {
        ht_entry_t *current_entry = inventory->warehouse->buckets[i].next;
        while (current_entry != NULL)
        {
            save_merch_to_file(current_entry->value.ptr_v, f);
            current_entry = current_entry->next;
        }
    }

    ioopm_linked_list_save_to_file(inventory->used_shelves, f);
    ll_entry_t *curr_entry = inventory->used_shelves->head;
    while (curr_entry)
    {
        char *shelf = curr_entry->value.ptr_v;
        fwrite(shelf, sizeof(char), strlen(shelf) + 1, f);
        curr_entry = curr_entry->next;
    }

    fclose(f);
}

ioopm_inventory_t *ioopm_inventory_load(char *file_name, heap_t *h)
{
    FILE *f = fopen(file_name, "rb");

    assert(f != NULL);

    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    bool is_empty = size == 0;
    fseek(f, 0, SEEK_SET);

    if (is_empty)
    {
        fclose(f);
        return ioopm_inventory_create(h);
    }

    ioopm_inventory_t *inventory = h_alloc_struct(h, "2*i");
    inventory->warehouse = ioopm_hash_table_load_from_file(h, f, string_sum_hash, string_compare_function, merch_compare_function, string_lt);

    for (int i = 0; i < inventory->warehouse->number_of_buckets; i++)
    {
        ht_entry_t *current_entry = inventory->warehouse->buckets[i].next;
        while (current_entry != NULL)
        {
            current_entry->value.ptr_v = load_merch_from_file(f, h);
            current_entry->key.ptr_v = ((inventory_merch_t *)current_entry->value.ptr_v)->name;
            current_entry = current_entry->next;
        }
    }

    inventory->used_shelves = ioopm_linked_list_load_from_file(f, string_compare_function, h);

    ll_entry_t *curr_entry = inventory->used_shelves->head;
    while (curr_entry)
    {
        int str_len = get_str_len_file(f);
        char *shelf = h_alloc_data(h, str_len +1); 
        fread(shelf, sizeof(char), str_len, f);
        curr_entry->value.ptr_v = shelf;
        curr_entry = curr_entry->next;
    }

    inventory->password = get_password();
    fclose(f);

    return inventory;
}

void ioopm_inventory_unstock(ioopm_inventory_t *inventory, char *merch_name, int amount, void *shelf, heap_t *h)
{
    inventory_merch_t *merch = ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = merch_name})->ptr_v;
    ioopm_list_t *storage_list = merch->storage_locations;
    ioopm_list_iterator_t *iter = ioopm_list_iterator(storage_list, h);
    merch->total_stock -= amount;
    if (shelf != NULL)
    {
        storage_location_t *str_loc;
        while (true)
        {
            storage_location_t *cur_str_loc = ioopm_iterator_current(iter).ptr_v;
            if (strcmp(cur_str_loc->shelf, shelf) == 0)
            {
                str_loc = cur_str_loc;
                break;
            }
            if (!ioopm_iterator_has_next(iter))
            {
                break;
            }
            ioopm_iterator_next(iter);
        }

        str_loc->stock -= amount;
    }

    else
    {
        while (amount > 0)
        {
            storage_location_t *cur_str_loc = ioopm_iterator_current(iter).ptr_v;

            if (cur_str_loc->stock > amount)
            {
                cur_str_loc->stock -= amount;
                amount = 0;
            }
            else
            {
                amount -= cur_str_loc->stock;
                cur_str_loc->stock = 0;
            }

            ioopm_iterator_next(iter);

            if (cur_str_loc->stock <= 0)
            {
                ioopm_linked_list_remove(inventory->used_shelves, ioopm_linked_list_get_index(inventory->used_shelves, (elem_t){.ptr_v = cur_str_loc->shelf}));

                ioopm_linked_list_remove(storage_list, 0);
            }
        }
    }
}

ioopm_inventory_t *ioopm_inventory_create(heap_t *h)
{
    ioopm_inventory_t *inventory = h_alloc_struct(h, "2*i");
    inventory->warehouse = ioopm_hash_table_create_spec(0.75, 50, string_sum_hash, string_compare_function, merch_compare_function, string_lt, h);
    inventory->used_shelves = ioopm_linked_list_create(string_compare_function, h);
    inventory->password = get_password();

    return inventory;
}