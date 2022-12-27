#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mover.h"
#include "structs.h"
#include "hash_table.h"
#include "hash_table_internal.h"

int init_suite(void)
{
	// Change this function if you want to do something *before* you
	// run a test suite
	return 0;
}

int clean_suite(void)
{
	// Change this function if you want to do something *after* you
	// run a test suite
	return 0;
}

void test_do_move()
{
	internal_heap_t *i_heap = h_init_internal(2, 128);

	make_active(i_heap->page_arr[0]);

	struct s1
	{
		int i;
		int j;
		char c;
		int *ptr;
	};

	struct s2
	{
		char c;
		int i;
		int *ptr;
		float f;
		struct s1 *linked_struct;
	};

	bool format_vector2[] = {0, 1, 0, 1};

	struct s2 *ss2 = page_alloc_struct(i_heap->page_arr[0], format_vector2, 4, 32);

	ss2->c = 'y';
	ss2->i = 987654;
	ss2->ptr = page_alloc_data(i_heap->page_arr[0], 8);
	ss2->f = 5.765000;

	*ss2->ptr = 45678;

	bool format_vector1[] = {0, 0, 1};

	ss2->linked_struct = page_alloc_struct(i_heap->page_arr[0], format_vector1, 3, 24);

	ss2->linked_struct->i = 5;
	ss2->linked_struct->j = 987654;
	ss2->linked_struct->c = 'h';
	ss2->linked_struct->ptr = page_alloc_data(i_heap->page_arr[0], 8);

	*ss2->linked_struct->ptr = 45678;

	CU_ASSERT_FALSE(is_active(i_heap->page_arr[1]));

	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss2));
	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss2));

	CU_ASSERT_TRUE(ss2->c == 'y');
	CU_ASSERT_TRUE(ss2->i == 987654);
	CU_ASSERT_TRUE(*ss2->ptr == 45678);

	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss2->linked_struct));
	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss2->linked_struct));

	CU_ASSERT_TRUE(ss2->linked_struct->c == 'h');
	CU_ASSERT_TRUE(ss2->linked_struct->i == 5);
	CU_ASSERT_TRUE(ss2->linked_struct->j == 987654);
	CU_ASSERT_TRUE(*ss2->linked_struct->ptr == 45678);

	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss2->ptr));
	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss2->ptr));

	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss2->linked_struct->ptr));
	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss2->linked_struct->ptr));

	do_move((void **)&ss2, &i_heap->page_arr[1], 1, NULL, 0, i_heap);

	CU_ASSERT_TRUE(is_active(i_heap->page_arr[1]));

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[1], ss2));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss2));

	CU_ASSERT_TRUE(ss2->c == 'y');
	CU_ASSERT_TRUE(ss2->i == 987654);
	CU_ASSERT_TRUE(*ss2->ptr == 45678);

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[1], ss2->linked_struct));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss2->linked_struct));

	CU_ASSERT_TRUE(ss2->linked_struct->c == 'h');
	CU_ASSERT_TRUE(ss2->linked_struct->i == 5);
	CU_ASSERT_TRUE(ss2->linked_struct->j == 987654);
	CU_ASSERT_TRUE(*ss2->linked_struct->ptr == 45678);

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[1], ss2->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss2->ptr));

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[1], ss2->linked_struct->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss2->linked_struct->ptr));

	h_delete_internal(i_heap);
}

void test_do_move_ref_loop()
{
	struct s1;
	struct s2;
	struct s3;

	struct s1
	{
		struct s2 *s_ptr;
		int i;
	};

	struct s2
	{
		struct s3 *s_ptr;
		int i;
	};

	struct s3
	{
		struct s1 *s_ptr;
		int i;
	};

	internal_heap_t *i_heap = h_init_internal(2, 128);

	make_active(i_heap->page_arr[0]);

	bool format_vector[] = {1, 0};

	struct s1 *ss1 = page_alloc_struct(i_heap->page_arr[0], format_vector, 2, 16);

	ss1->i = 567;
	ss1->s_ptr = page_alloc_struct(i_heap->page_arr[0], format_vector, 2, 16);

	ss1->s_ptr->i = 987;
	ss1->s_ptr->s_ptr = page_alloc_struct(i_heap->page_arr[0], format_vector, 2, 16);

	ss1->s_ptr->s_ptr->i = 2345;
	ss1->s_ptr->s_ptr->s_ptr = ss1;

	struct s1 *ss1_old = ss1;

	CU_ASSERT_FALSE(is_active(i_heap->page_arr[1]));

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss1));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss1));

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss1->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss1->s_ptr));

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss1->s_ptr->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss1->s_ptr->s_ptr));

	CU_ASSERT_PTR_EQUAL(ss1->s_ptr->s_ptr->s_ptr, ss1);

	do_move((void **)&ss1, &i_heap->page_arr[1], 1, NULL, 0, i_heap);

	CU_ASSERT_TRUE(is_active(i_heap->page_arr[1]));

	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss1));
	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[1], ss1));
	CU_ASSERT_EQUAL(ss1->i, 567);

	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss1->s_ptr));
	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[1], ss1->s_ptr));
	CU_ASSERT_EQUAL(ss1->s_ptr->i, 987);

	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss1->s_ptr->s_ptr));
	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[1], ss1->s_ptr->s_ptr));
	CU_ASSERT_EQUAL(ss1->s_ptr->s_ptr->i, 2345);

	CU_ASSERT_PTR_EQUAL(ss1->s_ptr->s_ptr->s_ptr, ss1);
	CU_ASSERT_PTR_NOT_EQUAL(ss1, ss1_old);

	h_delete_internal(i_heap);
}

void test_do_move_static_pages()
{
	struct s1;
	struct s2;
	struct s3;

	struct s1
	{
		struct s2 *s_ptr;
		int i;
		char *ptr;
	};

	struct s2
	{
		struct s3 *s_ptr;
		int i;
		char *ptr;
	};

	struct s3
	{
		struct s1 *s_ptr;
		int i;
		char *ptr;
	};

	internal_heap_t *i_heap = h_init_internal(4, 128);

	page_t *passive_pages[] = {i_heap->page_arr[2], i_heap->page_arr[3]};

	make_active(i_heap->page_arr[0]);
	make_active(i_heap->page_arr[1]);

	bool format_vector[] = {1, 0, 1};

	struct s1 *ss1 = page_alloc_struct(i_heap->page_arr[0], format_vector, 3, 24);
	ss1->i = 76;
	ss1->ptr = page_alloc_data(i_heap->page_arr[0], 8);
	*ss1->ptr = 'a';

	ss1->s_ptr = page_alloc_struct(i_heap->page_arr[1], format_vector, 3, 24);
	ss1->s_ptr->i = 45;
	ss1->s_ptr->ptr = page_alloc_data(i_heap->page_arr[1], 8);
	*ss1->s_ptr->ptr = 'g';

	ss1->s_ptr->s_ptr = page_alloc_struct(i_heap->page_arr[0], format_vector, 3, 24);
	ss1->s_ptr->s_ptr->i = 765;
	ss1->s_ptr->s_ptr->ptr = page_alloc_data(i_heap->page_arr[1], 8);
	*ss1->s_ptr->s_ptr->ptr = 't';

	ss1->s_ptr->s_ptr->s_ptr = ss1;

	do_move((void **)&ss1, passive_pages, 2, &i_heap->page_arr[0], 1, i_heap);

	CU_ASSERT_TRUE(is_active(i_heap->page_arr[2]));
	CU_ASSERT_FALSE(is_active(i_heap->page_arr[3]));

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss1));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss1));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[2], ss1));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[3], ss1));
	CU_ASSERT_EQUAL(ss1->i, 76);

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss1->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss1->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[2], ss1->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[3], ss1->ptr));
	CU_ASSERT_EQUAL(*ss1->ptr, 'a');

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[2], ss1->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss1->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss1->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[3], ss1->s_ptr));
	CU_ASSERT_EQUAL(ss1->s_ptr->i, 45);

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[2], ss1->s_ptr->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss1->s_ptr->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss1->s_ptr->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[3], ss1->s_ptr->ptr));
	CU_ASSERT_EQUAL(*ss1->s_ptr->ptr, 'g');

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[0], ss1->s_ptr->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss1->s_ptr->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[2], ss1->s_ptr->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[3], ss1->s_ptr->s_ptr));
	CU_ASSERT_EQUAL(ss1->s_ptr->s_ptr->i, 765);

	CU_ASSERT_TRUE(is_ptr_to_page(i_heap->page_arr[2], ss1->s_ptr->s_ptr->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[1], ss1->s_ptr->s_ptr->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[0], ss1->s_ptr->s_ptr->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(i_heap->page_arr[3], ss1->s_ptr->s_ptr->ptr));
	CU_ASSERT_EQUAL(*ss1->s_ptr->s_ptr->ptr, 't');

	CU_ASSERT_PTR_EQUAL(ss1->s_ptr->s_ptr->s_ptr, ss1);

	h_delete_internal(i_heap);
}

void test_move()
{
	struct person
	{
		char *first_name;
		char *last_name;
		int age;
		long prs_num;
		double d;
	};

	struct house
	{
		int num_windows;
		char *city;
		char *wall_colour;
		int postcode;
		long price_in_ore;
		struct person *owner;
	};

	internal_heap_t *h = h_init_internal(6, 2048);

	struct person *pers = h_alloc_struct_internal(h, "**ild");
	pers->age = 69;
	pers->first_name = h_alloc_data_internal(h, 8);
	char *name = "Lennart";
	memcpy(pers->first_name, name, 8);
	pers->last_name = h_alloc_data_internal(h, 8);
	name = "Akesson";
	memcpy(pers->last_name, name, 8);
	pers->prs_num = 5308091327;
	pers->d = 556.234567;

	struct house *hus = h_alloc_struct_internal(h, "i**il*");
	hus->city = h_alloc_data_internal(h, 11);
	name = "Atvidaberg";
	memcpy(hus->city, name, 11);
	hus->num_windows = 1;
	hus->postcode = 20000;
	hus->price_in_ore = 7000000000;
	hus->wall_colour = h_alloc_data_internal(h, 7);
	name = "Orange";
	memcpy(hus->wall_colour, name, 7);
	hus->owner = pers;

	void **ptrs[] = {(void **)&hus, (void **)&pers};
	void *ptrs2[] = {(void *)hus, (void *)pers};
	struct house *old_hus = hus;
	struct person *old_pers = pers;

	move(h, ptrs, 2, false, ptrs2);

	CU_ASSERT_PTR_NOT_EQUAL_FATAL(hus, old_hus);
	CU_ASSERT_PTR_NOT_EQUAL_FATAL(pers, old_pers);

	CU_ASSERT_EQUAL(pers->age, 69);
	CU_ASSERT_STRING_EQUAL(pers->first_name, "Lennart");
	CU_ASSERT_STRING_EQUAL(pers->last_name, "Akesson");
	CU_ASSERT_EQUAL(pers->prs_num, 5308091327);
	CU_ASSERT_DOUBLE_EQUAL(pers->d, 556.234567, 0.0001);

	CU_ASSERT_EQUAL(hus->num_windows, 1);
	CU_ASSERT_EQUAL(hus->postcode, 20000);
	CU_ASSERT_EQUAL(hus->price_in_ore, 7000000000);
	CU_ASSERT_STRING_EQUAL(hus->city, "Atvidaberg");
	CU_ASSERT_STRING_EQUAL(hus->wall_colour, "Orange");
	CU_ASSERT_PTR_EQUAL(hus->owner, pers);

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[1], hus));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[0], hus));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[1], pers));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[0], pers));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[1], pers->first_name));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[0], pers->first_name));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[1], pers->last_name));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[0], pers->last_name));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[1], hus->city));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[0], hus->city));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[1], hus->wall_colour));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[0], hus->wall_colour));

	CU_ASSERT_TRUE(is_active(h->page_arr[1]));
	CU_ASSERT_FALSE(is_active(h->page_arr[0]));

	h_delete_internal(h);
}

void test_move_stationary()
{
	struct person
	{
		char *first_name;
		char *last_name;
		int age;
		long prs_num;
		double d;
	};

	struct house
	{
		int num_windows;
		char *city;
		char *wall_colour;
		int postcode;
		long price_in_ore;
		struct person *owner;
	};

	internal_heap_t *h = h_init_internal(6, 2048);

	struct person *pers = h_alloc_struct_internal(h, "**ild");
	pers->age = 69;
	pers->first_name = h_alloc_data_internal(h, 8);
	char *name = "Lennart";
	memcpy(pers->first_name, name, 8);
	pers->last_name = h_alloc_data_internal(h, 8);
	name = "Akesson";
	memcpy(pers->last_name, name, 8);
	pers->prs_num = 5308091327;
	pers->d = 556.234567;

	struct house *hus = h_alloc_struct_internal(h, "i**il*");
	hus->city = h_alloc_data_internal(h, 11);
	name = "Atvidaberg";
	memcpy(hus->city, name, 11);
	hus->num_windows = 1;
	hus->postcode = 20000;
	hus->price_in_ore = 7000000000;
	hus->wall_colour = h_alloc_data_internal(h, 7);
	name = "Orange";
	memcpy(hus->wall_colour, name, 7);
	hus->owner = pers;

	void **ptrs[] = {(void **)&hus, (void **)&pers};
	void *ptrs2[] = {(void *)hus, (void *)pers};
	struct house *old_hus = hus;
	struct person *old_pers = pers;

	move(h, ptrs, 2, true, ptrs2);

	CU_ASSERT_PTR_EQUAL_FATAL(hus, old_hus);
	CU_ASSERT_PTR_EQUAL_FATAL(pers, old_pers);

	CU_ASSERT_EQUAL(pers->age, 69);
	CU_ASSERT_STRING_EQUAL(pers->first_name, "Lennart");
	CU_ASSERT_STRING_EQUAL(pers->last_name, "Akesson");
	CU_ASSERT_EQUAL(pers->prs_num, 5308091327);
	CU_ASSERT_DOUBLE_EQUAL(pers->d, 556.234567, 0.0001);

	CU_ASSERT_EQUAL(hus->num_windows, 1);
	CU_ASSERT_EQUAL(hus->postcode, 20000);
	CU_ASSERT_EQUAL(hus->price_in_ore, 7000000000);
	CU_ASSERT_STRING_EQUAL(hus->city, "Atvidaberg");
	CU_ASSERT_STRING_EQUAL(hus->wall_colour, "Orange");
	CU_ASSERT_PTR_EQUAL(hus->owner, pers);

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[0], hus));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[1], hus));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[0], pers));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[1], pers));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[0], pers->first_name));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[1], pers->first_name));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[0], pers->last_name));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[1], pers->last_name));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[0], hus->city));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[1], hus->city));

	CU_ASSERT_TRUE(is_ptr_to_page(h->page_arr[0], hus->wall_colour));
	CU_ASSERT_FALSE(is_ptr_to_page(h->page_arr[1], hus->wall_colour));

	CU_ASSERT_TRUE(is_active(h->page_arr[0]));
	CU_ASSERT_FALSE(is_active(h->page_arr[1]));

	h_delete_internal(h);
}

void test_move_unions()
{

	union u
	{
		int *ptr;
		int i;
	};

	struct s
	{
		union u un;
		int j;
		int *ptr;
	};

	internal_heap_t *h = h_init_internal(2, 128);

	struct s *s1 = h_alloc_struct_internal(h, "*i*");
	struct s *old_s1 = s1;

	s1->j = 5;
	s1->ptr = h_alloc_data_internal(h, 8);
	int *old_ptr = s1->ptr;
	*s1->ptr = 6;
	s1->un.i = 7;

	void **ptrs[] = {(void **)&s1};
	void *proof_ptrs[] = {(void *)s1};

	CU_ASSERT_TRUE(h->num_active_pages == 1);

	move(h, ptrs, 1, false, proof_ptrs);

	CU_ASSERT_TRUE(h->num_active_pages == 1);
	CU_ASSERT_PTR_NOT_EQUAL(s1, old_s1);
	CU_ASSERT_PTR_NOT_EQUAL(s1->ptr, old_ptr);

	CU_ASSERT_EQUAL(s1->j, 5);
	CU_ASSERT_EQUAL(*s1->ptr, 6);
	CU_ASSERT_EQUAL(s1->un.i, 7);

	old_s1 = s1;
	old_ptr = s1->ptr;

	s1->un.ptr = h_alloc_data_internal(h, 8);

	*s1->un.ptr = 8;

	int *old_un_ptr = s1->un.ptr;

	ptrs[0] = (void **)&s1;
	proof_ptrs[0] = (void *)s1;

	CU_ASSERT_TRUE(h->num_active_pages == 1);

	move(h, ptrs, 1, false, proof_ptrs);

	CU_ASSERT_TRUE(h->num_active_pages == 1);
	CU_ASSERT_PTR_NOT_EQUAL(s1, old_s1);
	CU_ASSERT_PTR_NOT_EQUAL(s1->ptr, old_ptr);
	CU_ASSERT_PTR_NOT_EQUAL(s1->un.ptr, old_un_ptr);

	CU_ASSERT_EQUAL(s1->j, 5);
	CU_ASSERT_EQUAL(*s1->ptr, 6);
	CU_ASSERT_EQUAL(*s1->un.ptr, 8);

	h_delete_internal(h);
}

int simple_hash_int(elem_t key, int buckets)
{
	return abs(key.int_v % buckets);
}

bool compare_eq_int(elem_t a, elem_t b)
{
	return a.int_v == b.int_v;
}

bool compare_lt_int(elem_t a, elem_t b)
{
	return a.int_v < b.int_v;
}

void test_do_move_ht_no_entries()
{
	heap_t *h = h_init(8192, false, 1.0);

	ioopm_hash_table_t *ht = ioopm_hash_table_create(simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

	ioopm_hash_table_t *old_ht = ht;

	CU_ASSERT_EQUAL(h_used(h), 528);

	for (size_t i = 0; i < 95; i++)
	{
		h_alloc_data(h, 1);
	}

	CU_ASSERT_EQUAL(h_used(h), 2048);

	CU_ASSERT_EQUAL(h->internal_heap->num_active_pages, 1);

	do_move((void **)&ht, &h->internal_heap->page_arr[1], 1, NULL, 0, h->internal_heap);

	CU_ASSERT_PTR_NOT_EQUAL(ht, old_ht);
	CU_ASSERT_PTR_NOT_EQUAL(ht->buckets, old_ht->buckets);
	CU_ASSERT_PTR_EQUAL(ht->compare_equal_keys, old_ht->compare_equal_keys);
	CU_ASSERT_PTR_EQUAL(ht->compare_equal_values, old_ht->compare_equal_values);
	CU_ASSERT_PTR_EQUAL(ht->compare_lessthan_keys, old_ht->compare_lessthan_keys);
	CU_ASSERT_PTR_EQUAL(ht->h_fnc, old_ht->h_fnc);
	CU_ASSERT_EQUAL(ht->load_factor, old_ht->load_factor);
	CU_ASSERT_EQUAL(ht->capacity, old_ht->capacity);
	CU_ASSERT_EQUAL(ht->NO_entries, old_ht->NO_entries);
	CU_ASSERT_EQUAL(ht->number_of_buckets, old_ht->number_of_buckets);

	CU_ASSERT_EQUAL(h->internal_heap->num_active_pages, 2);

	CU_ASSERT_EQUAL(h_used(h), 2576);

	h_delete(h);
}

void test_do_move_ht_one_entry()
{
	heap_t *h = h_init(8192, false, 1.0);

	ioopm_hash_table_t *ht = ioopm_hash_table_create(simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

	CU_ASSERT_EQUAL_FATAL(h_used(h), 528);

	for (size_t i = 0; i < 95; i++)
	{
		h_alloc_data(h, 1);
	}

	CU_ASSERT_EQUAL_FATAL(h_used(h), 2048);
	CU_ASSERT_EQUAL_FATAL(h->internal_heap->num_active_pages, 1);

	ioopm_hash_table_insert(ht, (elem_t){.int_v = 6}, (elem_t){.int_v = 7}, h);

	CU_ASSERT_EQUAL_FATAL(h_used(h), 2080);
	CU_ASSERT_EQUAL_FATAL(h->internal_heap->num_active_pages, 2);

	ioopm_hash_table_t *old_ht = ht;

	int bucket = 6 % ioopm_hash_table_number_of_buckets(ht);

	CU_ASSERT_EQUAL_FATAL(ht->buckets[bucket].next->key.int_v, 6);
	CU_ASSERT_EQUAL_FATAL(ht->buckets[bucket].next->value.int_v, 7);

	do_move((void **)&ht, &h->internal_heap->page_arr[2], 1, NULL, 0, h->internal_heap);

	CU_ASSERT_EQUAL(h_used(h), 2640);
	CU_ASSERT_EQUAL(h->internal_heap->num_active_pages, 3);

	CU_ASSERT_EQUAL(ht->buckets[bucket].next->key.int_v, 6);
	CU_ASSERT_EQUAL(ht->buckets[bucket].next->value.int_v, 7);

	CU_ASSERT_PTR_NOT_EQUAL(ht, old_ht);
	CU_ASSERT_PTR_NOT_EQUAL(ht->buckets, old_ht->buckets);
	CU_ASSERT_PTR_NOT_EQUAL(ht->buckets[bucket].next, old_ht->buckets[bucket].next);

	CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t){.int_v = 6})->int_v, 7);

	h_delete(h);
}

void test_move_ht_one_entry()
{
	heap_t *h = h_init(8192, false, 1.0);

	ioopm_hash_table_t *ht = ioopm_hash_table_create(simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

	CU_ASSERT_EQUAL_FATAL(h_used(h), 528);

	for (size_t i = 0; i < 95; i++)
	{
		h_alloc_data(h, 1);
	}

	CU_ASSERT_EQUAL_FATAL(h_used(h), 2048);
	CU_ASSERT_EQUAL_FATAL(h->internal_heap->num_active_pages, 1);

	ioopm_hash_table_insert(ht, (elem_t){.int_v = 6}, (elem_t){.int_v = 7}, h);

	CU_ASSERT_EQUAL_FATAL(h_used(h), 2080);
	CU_ASSERT_EQUAL_FATAL(h->internal_heap->num_active_pages, 2);

	int bucket = 6 % ioopm_hash_table_number_of_buckets(ht);

	ht_entry_t *old_entry = ht->buckets[bucket].next;

	CU_ASSERT_EQUAL_FATAL(ht->buckets[bucket].next->key.int_v, 6);
	CU_ASSERT_EQUAL_FATAL(ht->buckets[bucket].next->value.int_v, 7);

	void **ptrs[] = {(void **)&ht};
	void *ctrl_ptrs[] = {(void *)ht};

	move(h->internal_heap, ptrs, 1, true, ctrl_ptrs);

	CU_ASSERT_EQUAL(h_used(h), 2080);
	CU_ASSERT_EQUAL(h->internal_heap->num_active_pages, 2);

	CU_ASSERT_EQUAL(ht->buckets[bucket].next->key.int_v, 6);
	CU_ASSERT_EQUAL(ht->buckets[bucket].next->value.int_v, 7);

	CU_ASSERT_PTR_NOT_EQUAL(ht->buckets[bucket].next, old_entry);

	CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t){.int_v = 6})->int_v, 7);

	h_delete(h);
}

void test_move_ht_many_entries()
{
	heap_t *h = h_init(8192, true, 1.0);

	ioopm_hash_table_t *ht = ioopm_hash_table_create(simple_hash_int, compare_eq_int, compare_eq_int, compare_lt_int, h);

	CU_ASSERT_EQUAL_FATAL(h_used(h), 528);

	for (size_t i = 0; i < 95; i++)
	{
		h_alloc_data(h, 1);
	}

	CU_ASSERT_EQUAL_FATAL(h_used(h), 2048);
	CU_ASSERT_EQUAL_FATAL(h->internal_heap->num_active_pages, 1);

	int buckets[64];

	for (size_t i = 0; i < 64; i++)
	{
		ioopm_hash_table_insert(ht, (elem_t){.int_v = i}, (elem_t){.int_v = (i+1)*2}, h);
		CU_ASSERT_EQUAL_FATAL(h_used(h), 2048 + 32*(i+1));
		CU_ASSERT_EQUAL_FATAL(h->internal_heap->num_active_pages, 2);

		buckets[i] = i % ioopm_hash_table_number_of_buckets(ht);

		ht_entry_t *next_entry = ht->buckets[buckets[i]].next;

		while (next_entry->next != NULL)
		{
			next_entry = next_entry->next;
		}

		ht_entry_t *old_entry = next_entry;

		CU_ASSERT_EQUAL_FATAL(next_entry->key.int_v, i);
		CU_ASSERT_EQUAL_FATAL(next_entry->value.int_v, (i+1)*2);

		void **ptrs[] = {(void **)&ht};
		void *ctrl_ptrs[] = {(void *)ht};

		move(h->internal_heap, ptrs, 1, true, ctrl_ptrs);

		next_entry = ht->buckets[buckets[i]].next;

		while (next_entry->next != NULL)
		{
			next_entry = next_entry->next;
		}

		CU_ASSERT_EQUAL(h_used(h), 2048 + 32*(i+1));
		CU_ASSERT_EQUAL(h->internal_heap->num_active_pages, 2);

		CU_ASSERT_EQUAL(next_entry->key.int_v, i);
		CU_ASSERT_EQUAL(next_entry->value.int_v, 2*(i+1));

		CU_ASSERT_PTR_NOT_EQUAL(next_entry, old_entry);

		printf("\nentry: %p\nkey: %d\nvalue: %d\nptr: %p\n", next_entry, next_entry->key.int_v, next_entry->value.int_v, next_entry->next);
		CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t){.int_v = i})->int_v, 2*(i+1));
	}

	for (size_t i = 0; i < 64; i++)
	{
		CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t){.int_v = i})->int_v, 2*(i+1));
	}

	h_delete(h);
}

int main()
{
	// First we try to set up CUnit, and exit if we fail
	if (CU_initialize_registry() != CUE_SUCCESS)
		return CU_get_error();

	// We then create an empty test suite and specify the name and
	// the init and cleanup functions
	CU_pSuite my_test_suite = CU_add_suite("Tests for mover functions", init_suite, clean_suite);
	if (my_test_suite == NULL)
	{
		// If the test suite could not be added, tear down CUnit and exit
		CU_cleanup_registry();
		return CU_get_error();
	}

	// This is where we add the test functions to our test suite.
	// For each call to CU_add_test we specify the test suite, the
	// name or description of the test, and the function that runs
	// the test in question. If you want to add another test, just
	// copy a line below and change the information

	if ((CU_add_test(my_test_suite, "Test for do_move with 2 structs", test_do_move) == NULL) ||
		(CU_add_test(my_test_suite, "Test for do_move with 3 structs in loop", test_do_move_ref_loop) == NULL) ||
		(CU_add_test(my_test_suite, "Test for do_move with a static page", test_do_move_static_pages) == NULL) ||
		(CU_add_test(my_test_suite, "Test for basic move", test_move) == NULL) ||
		(CU_add_test(my_test_suite, "Test for trying move but remain stationary", test_move_stationary) == NULL) ||
		(CU_add_test(my_test_suite, "Test for move with a union ", test_move_unions) == NULL) ||
		(CU_add_test(my_test_suite, "Test for do_move with a hash table with no entries", test_do_move_ht_no_entries) == NULL) ||
		(CU_add_test(my_test_suite, "Test for do_move with a hash table with one entry", test_do_move_ht_one_entry) == NULL) ||
		(CU_add_test(my_test_suite, "Test for move with a hash table with one entry", test_move_ht_one_entry) == NULL) ||
		(CU_add_test(my_test_suite, "Test for move with a hash table with many entries", test_move_ht_many_entries) == NULL) ||

		0)

	{
		// If adding any of the tests fails, we tear down CUnit and exit
		CU_cleanup_registry();
		return CU_get_error();
	}

	// Set the running mode. Use CU_BRM_VERBOSE for maximum output.
	// Use CU_BRM_NORMAL to only print errors and a summary
	CU_basic_set_mode(CU_BRM_VERBOSE);

	// This is where the tests are actually run!
	CU_basic_run_tests();

	// Tear down CUnit before exiting
	CU_cleanup_registry();
	return CU_get_error();
}
