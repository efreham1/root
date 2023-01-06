#include <CUnit/Basic.h>
#include <stdlib.h>
#include "gc.h"
#include "input_handler.h"
#include "heap.h"
#include "structs.h"
#include "hash_table.h"
#include "hash_table_internal.h"
#include "metadata.h"

int init_suite (void)
{
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite (void)
{
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}

void test_create_destroy (void)
{
  heap_t *h = h_init (10, false, 0.5);

  CU_ASSERT_PTR_NOT_NULL (h);

  h_delete (h);
}

void test_h_delete_dbg (void)
{
  heap_t *h = h_init (32, false, 0.5);

  CU_ASSERT_PTR_NOT_NULL_FATAL (h);

  void *ptr_data = h_alloc_data (h, 8);
  void *ptr_struct = h_alloc_struct (h, "i2*f3c");

  CU_ASSERT_PTR_NOT_NULL (ptr_data);
  CU_ASSERT_PTR_NOT_NULL (ptr_struct);

  h_delete_dbg (h, NULL);

  CU_ASSERT_PTR_NULL (ptr_data);
  CU_ASSERT_PTR_NULL (ptr_struct);
}

void test_alloc_struct (void)
{
  struct s1 {
      char c1;
      char c2;
      char c3;
      char c4;
      char c5;
      int i;
      float *ptr;
  };

  heap_t *h = h_init (200, false, 0.5);

  float bruh = 5.674;

  struct s1 *ss1 = h_alloc_struct (h, "5ci*");

  ss1->c1 = 'a';
  ss1->c2 = 'b';
  ss1->c3 = 'c';
  ss1->c4 = 'd';
  ss1->c5 = 'e';
  ss1->i = 675;
  ss1->ptr = &bruh;

  CU_ASSERT_EQUAL (ss1->c1, 'a');
  CU_ASSERT_EQUAL (ss1->c2, 'b');
  CU_ASSERT_EQUAL (ss1->c3, 'c');
  CU_ASSERT_EQUAL (ss1->c4, 'd');
  CU_ASSERT_EQUAL (ss1->c5, 'e');
  CU_ASSERT_EQUAL (ss1->i, 675);
  CU_ASSERT_PTR_EQUAL (ss1->ptr, &bruh);

  h_delete (h);
}

void test_alloc_data (void)
{
  heap_t *h = h_init (200, false, 0.5);

  int *data = h_alloc_data (h, sizeof (int));

  *data = 5;

  CU_ASSERT_PTR_NOT_NULL (data);
  CU_ASSERT_TRUE (*data == 5);

  h_delete (h);
}

void test_h_gc (void)
{
  heap_t *h = h_init (6000, true, 0.5);

  int *data_1 = h_alloc_data (h, sizeof (int));

  CU_ASSERT_PTR_NOT_NULL (data_1);

  data_1 = NULL;

  unsigned int bytes_collected = h_gc (h);

  CU_ASSERT_TRUE (0 < bytes_collected);

  h_delete (h);
}

void test_h_gc_multiple ()
{
  heap_t *h = h_init (6000, true, 0.5);

  int *data_1 = h_alloc_data (h, sizeof (int));
  int *data_2 = h_alloc_data (h, sizeof (int));

  CU_ASSERT_PTR_NOT_NULL (data_1);
  CU_ASSERT_PTR_NOT_NULL (data_2);

  for (size_t i = 0; i < 126; i++)
    {
      data_1 = NULL;

      unsigned int bytes_collected = h_gc (h);

      CU_ASSERT_TRUE (0 == bytes_collected);
      CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, 1);

      data_1 = h_alloc_data (h, sizeof (int));
    }
  for (size_t i = 0; i < 10; i++)
    {
      data_1 = h_alloc_data (h, sizeof (int));
      data_1 = NULL;

      CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, 2);

      unsigned int bytes_collected = h_gc (h);

      CU_ASSERT_EQUAL (bytes_collected, 16);
      CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, 1);
    }
  h_delete (h);
}

void test_h_gc_no_collecting ()
{
  heap_t *h = h_init (6000, true, 1.0);

  int *ptrs[383];

  for (size_t i = 0; i < 383; i++)
    {
      ptrs[i] = h_alloc_data (h, 8);

      CU_ASSERT_PTR_NOT_NULL (ptrs[i]);

      unsigned int bytes_collected = h_gc (h);

      CU_ASSERT_TRUE (0 == bytes_collected);
      int num_act = i / 128 + 1;
      CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, num_act);
    }
  h_delete (h);
}

void test_h_gc_dbg (void)
{
  heap_t *h = h_init (6000, false, 0.5);

  int *data_1 = h_alloc_data (h, sizeof (int));
  int *data_2 = h_alloc_data (h, sizeof (int));

  CU_ASSERT_PTR_NOT_NULL (data_2);

  data_1 = NULL;

  unsigned int bytes_collected = h_gc (h);

  int len = 0;
  page_t **act_pages = get_active_pages (h->internal_heap, &len);
  CU_ASSERT_EQUAL_FATAL (1, len);
  CU_ASSERT_TRUE_FATAL (is_ptr_to_page (act_pages[0], data_2));

  CU_ASSERT_TRUE (0 < bytes_collected);

  data_1 = h_alloc_data (h, sizeof (int));

  CU_ASSERT_PTR_NOT_NULL (data_1);

  data_1 = NULL;

  bytes_collected = h_gc_dbg (h, true);

  CU_ASSERT_TRUE (0 == bytes_collected);
  CU_ASSERT_PTR_NOT_NULL (data_2);

  free (act_pages);
  h_delete (h);
}

void test_auto_gc ()
{
  heap_t *h = h_init (2048, true, 0.5);

  for (size_t i = 0; i < 63; i++)
    {
      h_alloc_data (h, 8);
    }

  CU_ASSERT_EQUAL (h_used (h), 1008);
  h_alloc_data (h, 16);
  CU_ASSERT_EQUAL (h_used (h), 24);

  h_delete (h);
}

void test_auto_gc_unsafe_ptr ()
{

  struct s {
      int *ptr;
      int i;
  };

  heap_t *h = h_init (4096, true, 0.75);

  for (size_t i = 0; i < 64; i++)
    {
      h_alloc_data (h, 8);
    }

  int *data = h_alloc_data (h, 8);
  *data = 5;

  for (size_t i = 0; i < 63; i++)
    {
      h_alloc_data (h, 8);
    }
  CU_ASSERT_EQUAL_FATAL (h_used (h), 2048);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 1);

  struct s *s1 = h_alloc_struct (h, "*i");
  s1->ptr = data;
  s1->i = 7;
  data = NULL;

  for (size_t i = 0; i < 62; i++)
    {
      h_alloc_data (h, 8);
    }
  CU_ASSERT_EQUAL_FATAL (h_used (h), 3064);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 2);

  h_alloc_data (h, 8);

  CU_ASSERT_EQUAL (h_used (h), 1048);
  CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, 2);
  CU_ASSERT_EQUAL (s1->i, 7);
  CU_ASSERT_EQUAL (*s1->ptr, 5);

  h_delete (h);
}

void test_auto_gc_no_collection ()
{
  heap_t *h = h_init (8192, true, 0.01);

  int *ptrs[512];

  for (size_t i = 0; i < 512; i++)
    {
      ptrs[i] = h_alloc_data (h, 8);

      CU_ASSERT_PTR_NOT_NULL (ptrs[i]);

      int num_act = i / 128 + 1;
      CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, num_act);
    }
  CU_ASSERT_EQUAL (h_avail (h), 0)
  h_delete (h);
}

void test_h_avail (void)
{
  heap_t *h = h_init (6000, false, 0.5);

  CU_ASSERT_EQUAL (h_avail (h), 6144);

  h_delete (h);
}

void test_h_used (void) // return used data (as an unsigned int)
{
  heap_t *h = h_init (6000, false, 0.5);

  CU_ASSERT_EQUAL (h_used (h), 0);

  h_delete (h);
}

void test_input_handler (void)
{
  char *s1 = handle_input ("10i2l**4*1c");
  char *s2 = handle_input ("gul lök");
  char *s3 = handle_input ("2*0L");
  char *s4 = handle_input ("**ll2ld3d4d");
  char *s5 = handle_input ("");

  CU_ASSERT_STRING_EQUAL ("iiiiiiiiiill******c", s1);

  CU_ASSERT_PTR_EQUAL ((char *) 0xDEADCAFEBABEBEEF, s2);

  CU_ASSERT_PTR_EQUAL ((char *) 0xDEADCAFEBABEBEEF, s3);

  CU_ASSERT_STRING_EQUAL ("**lllldddddddd", s4);

  CU_ASSERT_PTR_EQUAL ((char *) 0xDEADCAFEBABEBEEF, s5);

  free (s1);
  free (s4);
}

void test_get_actual_size ()
{
  heap_t *h = h_init (6000, false, 0.5);

  CU_ASSERT_EQUAL (get_heap_actual_size (h), get_internal_heap_actual_size (h->internal_heap) + sizeof (heap_t));

  h_delete (h);
}

int simple_hash_int (elem_t key, int buckets)
{
  return abs (key.int_v % buckets);
}

bool compare_eq_int (elem_t a, elem_t b)
{
  return a.int_v == b.int_v;
}

bool compare_lt_int (elem_t a, elem_t b)
{
  return a.int_v < b.int_v;
}

void test_h_gc_ht_one_entry_safe_sptrs ()
{
  heap_t *h = h_init (8192, false, 1.0);

  ioopm_hash_table_t *ht = ioopm_hash_table_create (simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 832);

  for (size_t i = 0; i < 76; i++)
    {
      h_alloc_data (h, 1);
    }

  CU_ASSERT_EQUAL_FATAL (h_used (h), 2048);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 1);

  ioopm_hash_table_insert (ht, (elem_t) {.int_v = 6}, (elem_t) {.int_v = 7}, h);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 2080);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 2);

  int bucket = 6 % ioopm_hash_table_number_of_buckets (ht);

  CU_ASSERT_EQUAL_FATAL (ht->buckets[bucket]->next->key.int_v, 6);
  CU_ASSERT_EQUAL_FATAL (ht->buckets[bucket]->next->value.int_v, 7);

  CU_ASSERT_EQUAL (h_gc (h), 1216);

  CU_ASSERT_EQUAL (h_used (h), 864);
  CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, 1);

  CU_ASSERT_EQUAL (ht->buckets[bucket]->next->key.int_v, 6);
  CU_ASSERT_EQUAL (ht->buckets[bucket]->next->value.int_v, 7);

  CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = 6})->int_v, 7);

  h_delete (h);
}

void test_h_gc_ht_one_entry_unsafe_sptrs ()
{
  heap_t *h = h_init (8192, true, 1.0);

  ioopm_hash_table_t *ht = ioopm_hash_table_create (simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 832);

  for (size_t i = 0; i < 76; i++)
    {
      h_alloc_data (h, 1);
    }

  CU_ASSERT_EQUAL_FATAL (h_used (h), 2048);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 1);

  ioopm_hash_table_insert (ht, (elem_t) {.int_v = 6}, (elem_t) {.int_v = 7}, h);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 2080);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 2);

  h_alloc_data (h, 1);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 2096);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 2);

  int bucket = 6 % ioopm_hash_table_number_of_buckets (ht);

  CU_ASSERT_EQUAL_FATAL (ht->buckets[bucket]->next->key.int_v, 6);
  CU_ASSERT_EQUAL_FATAL (ht->buckets[bucket]->next->value.int_v, 7);

  CU_ASSERT_EQUAL (h_gc (h), 16);

  CU_ASSERT_EQUAL (h_used (h), 2080);
  CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, 2);

  CU_ASSERT_EQUAL (ht->buckets[bucket]->next->key.int_v, 6);
  CU_ASSERT_EQUAL (ht->buckets[bucket]->next->value.int_v, 7);

  CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = 6})->int_v, 7);

  h_delete (h);
}

void test_auto_gc_ht_many_entries ()
{
  heap_t *h = h_init (8192, true, 0.01);

  ioopm_hash_table_t *ht = ioopm_hash_table_create (simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 832);

  for (size_t i = 0; i < 76; i++)
    {
      h_alloc_data (h, 1);
    }

  CU_ASSERT_EQUAL_FATAL (h_used (h), 2048);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 1);

  for (size_t i = 0; i < 64; i++)
    {
      ioopm_hash_table_insert (ht, (elem_t) {.int_v = i}, (elem_t) {.int_v = (i + 1) * 2}, h);
      //CU_ASSERT_EQUAL(h_used(h), 2048 + 32 * (i + 1));
      //CU_ASSERT_EQUAL_FATAL(h->internal_heap->num_active_pages, 2);

      h_alloc_data (h, 1);

      for (size_t j = 0; j < i + 1; j++)
        {
          // printf("\ni: %ld j: %ld number of buckets: %d\n", i, j, ioopm_hash_table_number_of_buckets(ht));
          CU_ASSERT_TRUE_FATAL (ioopm_hash_table_has_key (ht, (elem_t) {.int_v = j}));
          CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = j})->int_v, 2 * (j + 1));
        }
    }
  h_delete (h);
}

void test_h_gc_ht_many_entries ()
{
  heap_t *h = h_init (8192, true, 1.0);

  ioopm_hash_table_t *ht = ioopm_hash_table_create (simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 832);

  for (size_t i = 0; i < 76; i++)
    {
      h_gc (h);
      h_alloc_data (h, 1);
    }

  CU_ASSERT_EQUAL_FATAL (h_used (h), 2048);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 1);

  for (size_t i = 0; i < 64; i++)
    {
      h_gc (h);
      ioopm_hash_table_insert (ht, (elem_t) {.int_v = i}, (elem_t) {.int_v = (i + 1) * 2}, h);
      CU_ASSERT_EQUAL (h_used (h), 2048 + 32 * (i + 1));
      CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 2);

      h_gc (h);
      h_alloc_data (h, 1);

      for (size_t j = 0; j < i + 1; j++)
        {
          //printf("\ni: %ld j: %ld number of buckets: %d\n", i, j, ioopm_hash_table_number_of_buckets(ht));
          CU_ASSERT_TRUE_FATAL (ioopm_hash_table_has_key (ht, (elem_t) {.int_v = j}));
          CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = j})->int_v, 2 * (j + 1));
        }
    }
  h_delete (h);
}

void test_h_gc_ht_many_entries_2 ()
{
  heap_t *h = h_init (8192, true, 1.0);

  ioopm_hash_table_t *ht = ioopm_hash_table_create (simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 832);

  for (size_t i = 0; i < 76; i++)
    {
      h_alloc_data (h, 1);
    }

  CU_ASSERT_EQUAL_FATAL (h_used (h), 2048);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 1);

  for (size_t i = 0; i < 64; i++)
    {
      ioopm_hash_table_insert (ht, (elem_t) {.int_v = i}, (elem_t) {.int_v = (i + 1) * 2}, h);

      h_alloc_data (h, 1);

      for (size_t j = 0; j < i + 1; j++)
        {
          // printf("\ni: %ld j: %ld number of buckets: %d\n", i, j, ioopm_hash_table_number_of_buckets(ht));
          CU_ASSERT_TRUE_FATAL (ioopm_hash_table_has_key (ht, (elem_t) {.int_v = j}));
          CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = j})->int_v, 2 * (j + 1));
        }
    }

  h_gc (h);

  CU_ASSERT_EQUAL (h_used (h), 2048 + 32 * 64);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 2);

  for (size_t i = 0; i < 64; i++)
    {
      CU_ASSERT_TRUE_FATAL (ioopm_hash_table_has_key (ht, (elem_t) {.int_v = i}));
      CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = i})->int_v, 2 * (i + 1));
    }

  h_delete (h);
}

void test_auto_gc_ht_many_entries_safe_ptrs ()
{
  heap_t *h = h_init (8192, false, 0.001);

  ioopm_hash_table_t *ht = ioopm_hash_table_create (simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 832);

  for (size_t i = 0; i < 76; i++)
    {
      h_alloc_data (h, 1);
    }

  CU_ASSERT_EQUAL_FATAL (h_used (h), 848);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 1);

  for (size_t i = 0; i < 64; i++)
    {
      ioopm_hash_table_insert (ht, (elem_t) {.int_v = i}, (elem_t) {.int_v = (i + 1) * 2}, h);
      CU_ASSERT_EQUAL (h_used (h), 832 + 32 * (i + 1));

      h_alloc_data (h, 1);

      for (size_t j = 0; j < i + 1; j++)
        {
          //printf("\ni: %ld j: %ld number of buckets: %d\n", i, j, ioopm_hash_table_number_of_buckets(ht));
          CU_ASSERT_TRUE (ioopm_hash_table_has_key (ht, (elem_t) {.int_v = j}));
          CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = j})->int_v, 2 * (j + 1));
        }
    }
  CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, 2);
  h_delete (h);
}

void test_h_gc_ht_many_entries_safe_ptrs ()
{
  heap_t *h = h_init (8192, false, 1.0);

  ioopm_hash_table_t *ht = ioopm_hash_table_create (simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

  CU_ASSERT_EQUAL_FATAL (h_used (h), 832);

  for (size_t i = 0; i < 76; i++)
    {
      h_gc (h);
      h_alloc_data (h, 1);
    }

  CU_ASSERT_EQUAL_FATAL (h_used (h), 848);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 1);

  for (size_t i = 0; i < 64; i++)
    {
      h_gc (h);
      ioopm_hash_table_insert (ht, (elem_t) {.int_v = i}, (elem_t) {.int_v = (i + 1) * 2}, h);
      CU_ASSERT_EQUAL (h_used (h), 832 + 32 * (i + 1));

      h_gc (h);
      h_alloc_data (h, 1);

      for (size_t j = 0; j < i + 1; j++)
        {
          // printf("\ni: %ld j: %ld number of buckets: %d\n", i, j, ioopm_hash_table_number_of_buckets(ht));
          CU_ASSERT_TRUE_FATAL (ioopm_hash_table_has_key (ht, (elem_t) {.int_v = j}));
          CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = j})->int_v, 2 * (j + 1));
        }
    }
  CU_ASSERT_EQUAL (h->internal_heap->num_active_pages, 2);
  h_delete (h);
}

void test_h_gc_ht_many_entries_2_safe_ptrs ()
{
  heap_t *h = h_init (8192, false, 1.0);

  ioopm_hash_table_t *ht = ioopm_hash_table_create (simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);
  CU_ASSERT_EQUAL_FATAL (h_used (h), 832);

  for (size_t i = 0; i < 76; i++)
    {
      h_alloc_data (h, 1);
    }

  CU_ASSERT_EQUAL_FATAL (h_used (h), 2048);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 1);

  for (size_t i = 0; i < 64; i++)
    {
      ioopm_hash_table_insert (ht, (elem_t) {.int_v = i}, (elem_t) {.int_v = (i + 1) * 2}, h);

      h_alloc_data (h, 1);

      for (size_t j = 0; j < i + 1; j++)
        {
          // printf("\ni: %ld j: %ld number of buckets: %d\n", i, j, ioopm_hash_table_number_of_buckets(ht));
          CU_ASSERT_TRUE_FATAL (ioopm_hash_table_has_key (ht, (elem_t) {.int_v = j}));
          CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = j})->int_v, 2 * (j + 1));
        }
    }

  h_gc (h);

  CU_ASSERT_EQUAL (h_used (h), 832 + 32 * 64);
  CU_ASSERT_EQUAL_FATAL (h->internal_heap->num_active_pages, 2);

  for (size_t i = 0; i < 64; i++)
    {
      CU_ASSERT_TRUE_FATAL (ioopm_hash_table_has_key (ht, (elem_t) {.int_v = i}));
      CU_ASSERT_EQUAL (ioopm_hash_table_lookup (ht, (elem_t) {.int_v = i})->int_v, 2 * (i + 1));
    }

  h_delete (h);
}

int main ()
{
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry () != CUE_SUCCESS)
    return CU_get_error ();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite my_test_suite = CU_add_suite ("Tests for gc functions", init_suite, clean_suite);
  if (my_test_suite == NULL)
    {
      // If the test suite could not be added, tear down CUnit and exit
      CU_cleanup_registry ();
      return CU_get_error ();
    }

  // This is where we add the test functions to our test suite.
  // For each call to CU_add_test we specify the test suite, the
  // name or description of the test, and the function that runs
  // the test in question. If you want to add another test, just
  // copy a line below and change the information

  if ((CU_add_test (my_test_suite, "Test for create and destroy", test_create_destroy) == NULL) ||
      (CU_add_test (my_test_suite, "Test for destroy with DBG", test_h_delete_dbg) == NULL) ||
      (CU_add_test (my_test_suite, "Test for alloc of struct", test_alloc_struct) == NULL) ||
      (CU_add_test (my_test_suite, "Test for alloc of data", test_alloc_data) == NULL) ||

      (CU_add_test (my_test_suite, "Test for manual gc", test_h_gc) == NULL) ||
      (CU_add_test (my_test_suite, "Test for multiple manual gc-runs", test_h_gc_multiple) == NULL) ||
      (CU_add_test (my_test_suite, "Test for multiple manual gc-runs with no collecting", test_h_gc_no_collecting)
       == NULL) ||
      (CU_add_test (my_test_suite, "Test for manual gc on a hash table with one entry and safe stack pointers", test_h_gc_ht_one_entry_safe_sptrs)
       == NULL) ||
      (CU_add_test (my_test_suite, "Test for manual gc on a hash table with one entry and unsafe stack pointers", test_h_gc_ht_one_entry_unsafe_sptrs)
       == NULL) ||
      (CU_add_test (my_test_suite, "Test for many manual gc-runs on a hash table with many entries", test_h_gc_ht_many_entries)
       == NULL) ||
      (CU_add_test (my_test_suite, "Test for one manual gc-run on a hash table with many entries", test_h_gc_ht_many_entries_2)
       == NULL) ||
      (CU_add_test (my_test_suite, "Test for many manual gc-runs on a hash table with many entries and safe stack pointers", test_h_gc_ht_many_entries_safe_ptrs)
       == NULL) ||
      (CU_add_test (my_test_suite, "Test for one manual gc-run on a hash table with many entries and safe stack pointers", test_h_gc_ht_many_entries_2_safe_ptrs)
       == NULL) ||

      (CU_add_test (my_test_suite, "Test for manual gc_dbg", test_h_gc_dbg) == NULL) ||

      (CU_add_test (my_test_suite, "Test for automatic gc", test_auto_gc) == NULL) ||
      (CU_add_test (my_test_suite, "Test for automatic gc with unsafe ptr and movement", test_auto_gc_unsafe_ptr)
       == NULL) ||
      (CU_add_test (my_test_suite, "Test for automatic gc with no collection", test_auto_gc_no_collection) == NULL) ||
      (CU_add_test (my_test_suite, "Test for automatic gc on a hash table with many entries", test_auto_gc_ht_many_entries)
       == NULL) ||
      (CU_add_test (my_test_suite, "Test for automatic gc on a hash table with many entries and safe stack pointers", test_auto_gc_ht_many_entries_safe_ptrs)
       == NULL) ||

      (CU_add_test (my_test_suite, "Test for available space", test_h_avail) == NULL) ||
      (CU_add_test (my_test_suite, "Test for used space", test_h_used) == NULL) ||
      (CU_add_test (my_test_suite, "Test for the input handler", test_input_handler) == NULL) ||
      (CU_add_test (my_test_suite, "Test for actual size", test_get_actual_size) == NULL) ||

      0)
    {
      // If adding any of the tests fails, we tear down CUnit and exit
      CU_cleanup_registry ();
      return CU_get_error ();
    }

  // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
  // Use CU_BRM_NORMAL to only print errors and a summary
  CU_basic_set_mode (CU_BRM_VERBOSE);

  // This is where the tests are actually run!
  CU_basic_run_tests ();

  // Tear down CUnit before exiting
  CU_cleanup_registry ();
  return CU_get_error ();
}
