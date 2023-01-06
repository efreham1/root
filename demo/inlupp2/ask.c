#include "general_TUI.h"
#include "ask.h"
#include <ctype.h>

bool is_valid_merch (char *str, void *extra, heap_t *h)
{
  return ioopm_hash_table_has_key (((ioopm_inventory_t *) extra)->warehouse, (elem_t) {.ptr_v = str});
}

bool is_less_than_stock (char *str, void *extra, heap_t *h)
{
  int *stock = extra;
  if (!is_number (str)) return false;
  int amount = str_to_int (str, NULL, h).int_t;
  return amount <= *stock && amount > 0;
}

void ioopm_ask_cart_merch (ioopm_inventory_t *inventory, char **merch_name, int *cost, int *pieces, heap_t *h)
{
  char *question = "Please enter the name of the merchandise you would like to add to your cart";
  *merch_name = ask_question (question, is_valid_merch, inventory, str_to_str, NULL, h).str_t;
  inventory_merch_t *inv_merch = ioopm_hash_table_lookup (inventory->warehouse, (elem_t) {.ptr_v = *merch_name})->ptr_v;
  *cost = inv_merch->price;
  *pieces = ask_question ("Please enter how many of the merchandise you'd like to add.", is_less_than_stock, &inv_merch->theoretical_stock, str_to_int, NULL, h).int_t;
}

bool is_existing_cart_merch (char *str, void *extra, heap_t *h)
{
  return ioopm_linked_list_contains (((ioopm_cart_t *) extra)->names, (elem_t) {.ptr_v = str});
}

char *ioopm_ask_existing_cart_merch_name (ioopm_cart_t *cart, heap_t *h)
{
  return ask_question ("Enter an existing name for a merchandise", is_existing_cart_merch, cart, str_to_str, NULL, h).str_t;
}

static bool is_menu_choice_list (elem_t value, void *extra)
{
  int i = atoi (((char *) extra));

  return i == value.int_v;
}

static bool is_menu_choice (char *str, void *extra, heap_t *h)
{
  if (!is_number (str)) return false;

  ioopm_list_t *options = (ioopm_list_t *) extra;
  return ioopm_linked_list_any (options, is_menu_choice_list, str);
}

int ioopm_ask_menu (char *menu, ioopm_list_t *options, heap_t *h)
{
  return ask_question (menu, is_menu_choice, options, str_to_int, NULL, h).int_t;
}

bool ioopm_ask_user_access (heap_t *h)
{
  char *wants_access = ask_question_string ("Do you want to open the menu with user access?", h);
  if (toupper (*wants_access) != 'Y')
    {

      return false;
    }

  return true;
}

bool ioopm_ask_admin_access (int password, heap_t *h)
{
  char *wants_access = ask_question_string ("Do you want to open the menu with admin access?", h);
  if (toupper (*wants_access) != 'Y')
    {

      return false;
    }
  int entered_password = ask_question_int ("Enter the admin password", h);

  return entered_password == password;
}

bool is_new_inv_merch (char *str, void *extra, heap_t *h)
{
  return !ioopm_hash_table_has_key ((ioopm_hash_table_t *) extra, (elem_t) {.ptr_v = str});
}

void ioopm_ask_new_inventory_merch (ioopm_hash_table_t *warehouse, char **name, char **desc, int *price, heap_t *h)
{
  *name = ask_question ("Enter a unique name for the new merchandise", is_new_inv_merch, warehouse, str_to_str, NULL, h).str_t;
  *desc = ask_question_string ("Enter a description for the merchandise", h);
  *price = ask_question_u_int ("Enter a price in Öre for the merchandise", h);
}

bool is_existing_inv_merch (char *str, void *extra, heap_t *h)
{
  return ioopm_hash_table_has_key ((ioopm_hash_table_t *) extra, (elem_t) {.ptr_v = str});
}

char *ioopm_ask_existing_inventory_merch (ioopm_hash_table_t *warehouse, heap_t *h)
{
  return ask_question ("Enter an existing name for a merchandise", is_existing_inv_merch, warehouse, str_to_str, NULL, h).str_t;
}

static bool is_new_shelf (char *str, void *extra, heap_t *h)
{
  if (strlen (str) < 2) return false;
  bool is_shelf = isalpha (str[0]) && is_number (str + 1);
  str[0] = toupper (str[0]);
  return is_shelf && !ioopm_linked_list_contains ((ioopm_list_t *) extra, (elem_t) {.ptr_v=str});
}

static bool is_old_shelf (char *str, void *extra, heap_t *h)
{
  if (strlen (str) < 2) return false;
  bool is_shelf = isalpha (str[0]) && is_number (str + 1);
  str[0] = toupper (str[0]);
  return is_shelf && ioopm_linked_list_contains ((ioopm_list_t *) extra, (elem_t) {.ptr_v=str});
}

char *ioopm_ask_new_shelf (ioopm_list_t *used_shelves, heap_t *h)
{
  return ask_question ("Please enter a new shelf with the format e.g. d3 or D3", is_new_shelf, used_shelves, str_to_str, NULL, h).str_t;
}

char *ioopm_ask_old_shelf (ioopm_list_t *used_shelves, heap_t *h)
{
  return ask_question ("Please enter an existing shelf with the format e.g. d3 or D3", is_old_shelf, used_shelves, str_to_str, NULL, h).str_t;
}

int ioopm_ask_question_u_int (char *question, heap_t *h)
{
  return ask_question_u_int (question, h);
}

char *ioopm_ask_question_string (char *question, heap_t *h)
{
  return ask_question_string (question, h);
}

int ioopm_ask_question_int (char *question, heap_t *h)
{
  return ask_question_int (question, h);
}

int ioopm_ask_No_stock (ioopm_inventory_t *inventory, char *merch_name, char *shelf, heap_t *h)
{
  inventory_merch_t *merch = ioopm_hash_table_lookup (inventory->warehouse, (elem_t) {.ptr_v = merch_name})->ptr_v;
  ioopm_list_iterator_t *iter = ioopm_list_iterator (merch->storage_locations, h);
  int stock = 0;
  while (true)
    {
      storage_location_t *cur_str_loc = ioopm_iterator_current (iter).ptr_v;
      if (strcmp (cur_str_loc->shelf, shelf) == 0)
        {
          stock = cur_str_loc->stock;
          break;
        }
      if (!ioopm_iterator_has_next (iter))
        {
          break;
        }
      ioopm_iterator_next (iter);
    }
  return ask_question ("How many items would you like to unstock?", is_less_than_stock, &stock, str_to_int, NULL, h).int_t;
}

bool is_valid_cart (char *str, void *extra, heap_t *h)
{
  ioopm_list_t *cart_list = extra;
  if (!is_number (str)) return false;
  int idx = atoi (str) - 1;
  if (idx < 0 || idx >= ioopm_linked_list_size (cart_list)) return false;
  if (ioopm_linked_list_get (cart_list, idx).ptr_v == NULL) return false;
  return true;
}

ioopm_cart_t *ask_cart_number (ioopm_list_t *cart_list, heap_t *h)
{
  int idx =
      ask_question ("Which cart-number would you like to use?", is_valid_cart, cart_list, str_to_int, NULL, h).int_t
      - 1;
  return ioopm_linked_list_get (cart_list, idx).ptr_v;
}