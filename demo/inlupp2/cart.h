#include "hash_table.h"
#include "gc.h"
#pragma once

typedef struct cart ioopm_cart_t;

typedef struct cart_merchandise cart_merch_t;

struct cart_merchandise {
    char *name;
    int price;
    int pcs;
};

struct cart {
    ioopm_list_t *contents;
    ioopm_list_t *names;
    int cost;
};

/// @brief eq comparison for merchandise
bool eq_cart_merch (elem_t a, elem_t b);

/// @brief Create a new empty cart
/// @return An empty cart
ioopm_cart_t *ioopm_cart_create (heap_t *h);

/// @brief Add a merchandise to a cart
/// @param cart The cart where the merchandise will be added
/// @param merch The merchandise to be added
void ioopm_cart_add (ioopm_cart_t *cart, char *merch_name, int cost, int pieces, heap_t *h);

/// @brief Remove a merchandise from a cart
/// @param cart The cart where the merchandise will be removed from
/// @param merch Type of merchandise to be removed
/// @param No_merch Number of merchandise of said type to be removed
void ioopm_cart_remove (ioopm_cart_t *cart, char *merch_name);

/// @brief Get the cost of all merchandise in a cart
/// @param cart The cart from where the cost will be calculated
/// @return The cost of the contents in öre
int ioopm_cart_get_cost (ioopm_cart_t *cart);

/// @brief Clear the contents of a cart
/// @param cart The cart to be cleared
void ioopm_cart_clear (ioopm_cart_t *cart);

/// @brief Get a linked list of all merchandise in a cart
/// @param cart The cart to get the merchandise from
/// @return A linked list of all the merchandise
ioopm_list_t *ioopm_cart_get_merch (ioopm_cart_t *cart);
