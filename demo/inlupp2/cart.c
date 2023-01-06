#include "cart.h"
#include <string.h>

bool eq_cart_merch (elem_t a, elem_t b)
{
  return a.ptr_v == b.ptr_v;
}

bool eq_str (elem_t a, elem_t b)
{
  return strcmp (((char *) a.ptr_v), ((char *) b.ptr_v)) == 0;
}

static cart_merch_t *create_merch (char *merch_name, int cost, int pieces, heap_t *h)
{
  cart_merch_t *merch = h_alloc_struct (h, "*2i");
  merch->name = merch_name;
  merch->price = cost;
  merch->pcs = pieces;
  return merch;
}

ioopm_cart_t *ioopm_cart_create (heap_t *h)
{
  ioopm_cart_t *cart = h_alloc_struct (h, "2*i");
  cart->contents = ioopm_linked_list_create (eq_cart_merch, h);
  cart->names = ioopm_linked_list_create (eq_str, h);
  cart->cost = 0;
  return cart;
}

void ioopm_cart_add (ioopm_cart_t *cart, char *merch_name, int cost, int pieces, heap_t *h)
{
  cart_merch_t *merch = create_merch (merch_name, cost, pieces, h);
  cart->cost += merch->price * merch->pcs;
  ioopm_linked_list_append (cart->contents, (elem_t) {.ptr_v = merch}, h);
  ioopm_linked_list_append (cart->names, (elem_t) {.ptr_v = merch_name}, h);
}

void ioopm_cart_remove (ioopm_cart_t *cart, char *merch_name)
{
  int idx = ioopm_linked_list_get_index (cart->names, (elem_t) {.ptr_v = merch_name});
  ioopm_linked_list_remove (cart->names, idx);
  elem_t merch = ioopm_linked_list_remove (cart->contents, idx);
  cart->cost -= ((cart_merch_t *) merch.ptr_v)->price * ((cart_merch_t *) merch.ptr_v)->pcs;

}

int ioopm_cart_get_cost (ioopm_cart_t *cart)
{
  return cart->cost;
}

void ioopm_cart_clear (ioopm_cart_t *cart)
{
  ioopm_linked_list_clear (cart->contents);
  ioopm_linked_list_clear (cart->names);
  cart->cost = 0;
}

ioopm_list_t *ioopm_cart_get_merch (ioopm_cart_t *cart)
{
  return cart->contents;
}