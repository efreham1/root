#include "cart.h"
#include "inventory.h"
#include "hash_table.h"

#pragma once

/// @brief Ask in a TUI for a merchandise
/// @return The entered merchandise
void ioopm_ask_cart_merch(ioopm_inventory_t *inventory, char **merch_name, int *cost, int *pieces, heap_t *h);

/// @brief Ask for an existing merchandise's name
/// @param cart The cart where the merchandise has to exist
/// @return The retirived name
char *ioopm_ask_existing_cart_merch_name(ioopm_cart_t *cart, heap_t *h);

/// @brief Ask for a menu choice
/// @param menu The menu which will be displayed
/// @param options The possible menu options
/// @return The choosed option
int ioopm_ask_menu(char *menu, ioopm_list_t *options, heap_t *h);

/// @brief See if the user wants user access
/// @return True if user access is granted
bool ioopm_ask_user_access(heap_t *h);

/// @brief See if the user wants and can have admin access
/// @return True if admin access is granted
bool ioopm_ask_admin_access(int password, heap_t *h);

/// @brief Ask for a non-existing inventory merch
/// @param warehouse the warehouse where the merch shouldn't exist
/// @param name pointer to where the name should be written
/// @param desc pointer to where the description should be written
/// @param price pointer to where the price should be written
void ioopm_ask_new_inventory_merch(ioopm_hash_table_t *warehouse, char **name, char **desc, int *price, heap_t *h);

/// @brief Ask for an existing inventory merch
/// @param warehouse the warehouse where the merch should exist
/// @return the name of the merch
char *ioopm_ask_existing_inventory_merch(ioopm_hash_table_t *warehouse, heap_t *h);

/// @brief Ask for a non-existing shelf
/// @param used_shelves the list where the shelf shouldn't exist
/// @return the name of the shelf
char *ioopm_ask_new_shelf(ioopm_list_t *used_shelves, heap_t *h);

/// @brief Ask for an existing shelf
/// @param used_shelves the list where the shelf should exist
/// @return the name of the shelf
char *ioopm_ask_old_shelf(ioopm_list_t *used_shelves, heap_t *h);

/// @brief Ask a question with an unsigned int as an answer
/// @param question the question to be asked
/// @return the answer
int ioopm_ask_question_u_int(char *question, heap_t *h);

/// @brief Ask a question with a string as an answer
/// @param question the question to be asked
/// @return the answer
char *ioopm_ask_question_string(char *question, heap_t *h);

/// @brief Ask a question with an int as an answer
/// @param question the question to be asked
/// @return the answer
int ioopm_ask_question_int(char *question, heap_t *h);

/// @brief Ask an amount of stock of a merch that's no larger than the actual stock
/// @param inventory the inventory where the merch exists
/// @param merch_name the name of the merch
/// @param shelf the name of the shelf
/// @return the answered amount
int ioopm_ask_No_stock(ioopm_inventory_t *inventory, char *merch_name, char *shelf, heap_t *h);

/// @brief Ask which cart the user wants to access
/// @param cart_list the list of carts
/// @return the cart that the user picked
ioopm_cart_t *ask_cart_number(ioopm_list_t *cart_list, heap_t *h);