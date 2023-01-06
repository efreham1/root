#include "linked_list.h"
#include "iterator.h"
#include "data_structure.h"
#include <stdio.h>
#include "linked_list_internal.h"

static ll_entry_t *create_link (elem_t value, ll_entry_t *next, heap_t *h)
{
  ll_entry_t *link = h_alloc_struct (h, "2*");
  link->value = value;
  link->next = next;
  return link;
}

ioopm_list_t *ioopm_linked_list_create (ioopm_eq_function eq_fn, heap_t *h)
{
  ioopm_list_t *ll = h_alloc_struct (h, "2*l*");
  ll->head = NULL;
  ll->tail = NULL;
  ll->eq_fn = eq_fn;
  return ll;
}

void ioopm_linked_list_append (ioopm_list_t *ll, elem_t value, heap_t *h)
{
  ll_entry_t *new_tail = create_link (value, NULL, h);
  if (ll->head == NULL)
    {
      ll->head = new_tail;
      ll->tail = new_tail;
      ll->size++;
    }
  else
    {
      ll_entry_t *tmp = ll->tail;
      ll->tail = new_tail;
      tmp->next = new_tail;
      ll->size++;
    }
}

void ioopm_linked_list_prepend (ioopm_list_t *ll, elem_t value, heap_t *h)
{
  ll_entry_t *new_head = create_link (value, ll->head, h);
  ll->head = new_head;
  ll->size++;
  if (ll->size == 1)
    {
      ll->tail = ll->head;
    }
}

void ioopm_linked_list_insert (ioopm_list_t *ll, int index, elem_t value, heap_t *h)
{
  if (index > ll->size || index < 0)
    {
      return;
    }
  else if (index == 0)
    {
      ioopm_linked_list_prepend (ll, value, h);
    }
  else if (index == ll->size)
    {
      ioopm_linked_list_append (ll, value, h);
    }
  else
    {
      ll_entry_t *current = ll->head;
      for (int i = 0; i < index - 1; ++i)
        {
          current = current->next;
        }
      ll_entry_t *replaced = current->next;
      ll_entry_t *new_link = create_link (value, replaced, h);
      current->next = new_link;
      ll->size++;
    }
}

elem_t ioopm_linked_list_remove (ioopm_list_t *ll, int index)
{
  if (index >= ll->size || index < 0)
    {
      return (elem_t) {.int_v = (-1)};
    }
  else if (index == 0)
    {
      ll_entry_t *new_head = ll->head->next;
      elem_t removed_value = ll->head->value;

      ll->head = new_head;
      ll->size--;
      if (ioopm_linked_list_is_empty (ll))
        {
          ll->tail = ll->head;
        }
      return removed_value;
    }
  else if (index == ll->size - 1)
    {
      ll_entry_t *new_tail = ll->head;
      for (int i = 0; i < index - 1; ++i)
        {
          new_tail = new_tail->next;
        }
      elem_t removed_value = ll->tail->value;

      new_tail->next = NULL;
      ll->tail = new_tail;
      ll->size--;
      return removed_value;
    }
  else
    {
      ll_entry_t *current = ll->head;
      for (int i = 0; i < index - 1; ++i)
        {
          current = current->next;
        }
      ll_entry_t *tmp = current->next->next;
      elem_t removed_value = current->next->value;

      current->next = tmp;
      ll->size--;
      return removed_value;
    }
}

elem_t ioopm_linked_list_get (ioopm_list_t *ll, int index)
{
  if (index >= ll->size || index < 0)
    {
      return (elem_t) {.int_v = (-1)};
    }
  else if (index == 0)
    {
      return ll->head->value;
    }
  else if (index == ll->size - 1)
    {
      return ll->tail->value;
    }
  else
    {
      ll_entry_t *current = ll->head;
      for (int i = 0; i < index; ++i)
        {
          current = current->next;
        }
      return current->value;
    }
}

bool ioopm_linked_list_contains (ioopm_list_t *ll, elem_t element)
{
  ll_entry_t *current = ll->head;
  for (int i = 0; i < ll->size; ++i)
    {
      if (ll->eq_fn (current->value, element))
        {
          return true;
        }
      current = current->next;
    }
  return false;
}

int ioopm_linked_list_get_index (ioopm_list_t *ll, elem_t element)
{
  ll_entry_t *current = ll->head;
  for (int i = 0; i < ll->size; ++i)
    {
      if (ll->eq_fn (current->value, element))
        {
          return i;
        }
      current = current->next;
    }
  return -1;
}

size_t ioopm_linked_list_size (ioopm_list_t *ll)
{
  return ll->size;
}

bool ioopm_linked_list_is_empty (ioopm_list_t *ll)
{
  return ll->size == 0;
}

void ioopm_linked_list_clear (ioopm_list_t *ll)
{
  ll->size = 0;
  ll->head = NULL;
  ll->tail = NULL;
}

bool ioopm_linked_list_all (ioopm_list_t *ll, ioopm_predicate_ll prop, void *extra)
{
  ll_entry_t *current = ll->head;
  bool result = true;
  for (int i = 0; i < ll->size && result; ++i)
    {
      result = prop (current->value, extra);
      current = current->next;
    }
  return result;
}

bool ioopm_linked_list_any (ioopm_list_t *ll, ioopm_predicate_ll prop, void *extra)
{
  ll_entry_t *current = ll->head;
  for (int i = 0; i < ll->size; ++i)
    {
      if (prop (current->value, extra))
        {
          return true;
        }
      current = current->next;
    }
  return false;
}

void ioopm_linked_list_apply_to_all (ioopm_list_t *ll, ioopm_apply_function_ll fun, void *extra)
{
  ll_entry_t *current = ll->head;
  for (int i = 0; i < ll->size; ++i)
    {
      fun (&current->value, extra);
      current = current->next;
    }
}

ioopm_list_t *ioopm_linked_list_copy (ioopm_list_t *old_ll, heap_t *h)
{
  ioopm_list_iterator_t *iter = ioopm_list_iterator (old_ll, h);
  ioopm_list_t *new_list = ioopm_linked_list_create (old_ll->eq_fn, h);
  for (int i = 0; i < ioopm_linked_list_size (old_ll); i++)
    {
      ioopm_linked_list_append (new_list, ioopm_iterator_current (iter), h);
      ioopm_iterator_next (iter);
    }
  return new_list;
}

void ioopm_linked_list_save_to_file (ioopm_list_t *ll, FILE *f)
{
  fwrite (ll, sizeof (ioopm_list_t), 1, f);
  ll_entry_t *curr_entry = ll->head;
  while (curr_entry != NULL)
    {
      fwrite (curr_entry, sizeof (ll_entry_t), 1, f);
      curr_entry = curr_entry->next;
    }
}

ioopm_list_t *ioopm_linked_list_load_from_file (FILE *f, ioopm_eq_function eq_fn, heap_t *h)
{
  ioopm_list_t *ll = h_alloc_struct (h, "2*l*");
  fread (ll, sizeof (ioopm_list_t), 1, f);
  if (ll->head != NULL)
    {
      ll_entry_t *head = h_alloc_struct (h, "2*");
      fread (head, sizeof (ll_entry_t), 1, f);
      ll->head = head;
      ll_entry_t *prev_entry = ll->head;
      ll_entry_t *curr_entry = head->next;
      while (curr_entry != NULL)
        {
          curr_entry = h_alloc_struct (h, "2*");
          fread (curr_entry, sizeof (ll_entry_t), 1, f);
          prev_entry->next = curr_entry;
          prev_entry = curr_entry;
          curr_entry = curr_entry->next;
        }
      ll->tail = prev_entry;
    }
  ll->eq_fn = eq_fn;
  return ll;

}

ioopm_list_iterator_t *ioopm_list_iterator (ioopm_list_t *ll, heap_t *h)
{
  ioopm_list_iterator_t *iter = h_alloc_struct (h, "4*");
  iter->head = ll->head;
  iter->current = ll->head;
  iter->next = ll->head != NULL ? ll->head->next : NULL;
  iter->list = ll;
  return iter;
}

bool ioopm_iterator_has_next (ioopm_list_iterator_t *iter)
{
  return (iter->next != NULL);
}

elem_t ioopm_iterator_next (ioopm_list_iterator_t *iter)
{
  ll_entry_t *new_current = iter->current;
  ll_entry_t *new_next = iter->next;
  if (iter->next != NULL)
    {
      new_current = iter->next;
      new_next = iter->next->next;
    }
  iter->current = new_current;
  iter->next = new_next;

  return iter->current->value;
}

void ioopm_iterator_remove (ioopm_list_iterator_t *iter)
{
  //TODO (optional)
}

void ioopm_iterator_insert (ioopm_list_iterator_t *iter, elem_t element)
{
  //TODO (optional)
}

void ioopm_iterator_reset (ioopm_list_iterator_t *iter)
{
  iter->current = iter->head;
  iter->next = iter->current->next;
}

elem_t ioopm_iterator_current (ioopm_list_iterator_t *iter)
{
  return iter->current->value;
}