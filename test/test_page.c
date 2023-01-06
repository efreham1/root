#include <CUnit/Basic.h>
#include <stdlib.h>
#include "page.h"
#include "metadata.h"
#include "structs.h"

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

void test_create_destroy()
{

	void *memory_block = calloc(256, 1);
	page_t *p = page_init(256, memory_block);

	CU_ASSERT_PTR_NOT_NULL(p);

	page_delete(p);
	free(memory_block);
}

void test_is_make_active_passive()
{
	void *memory_block = calloc(128, 1);
	page_t *p = page_init(128, memory_block);

	CU_ASSERT_FALSE(is_active(p));

	make_active(p);

	CU_ASSERT_TRUE(is_active(p));

	make_passive(p);

	CU_ASSERT_FALSE(is_active(p));

	make_active(p);

	CU_ASSERT_TRUE(is_active(p));

	page_delete(p);
	free(memory_block);
}

void test_has_room()
{
	void *memory_block = calloc(64, 1);
	page_t *p = page_init(64, memory_block);

	make_active(p);

	CU_ASSERT_TRUE(has_room(p, 56));
	CU_ASSERT_FALSE(has_room(p, 57));

	page_delete(p);
	free(memory_block);
}

void test_page_alloc_data()
{
	void *memory_block = calloc(128, 1);
	page_t *p = page_init(128, memory_block);
	make_active(p);

	CU_ASSERT_TRUE(has_room(p, 120));
	CU_ASSERT_FALSE(has_room(p, 121));

	int *i = (int *)page_alloc_data(p, 8, 1);

	CU_ASSERT_PTR_NOT_NULL(i);

	*i = 76543;
	metadata_t *md = (metadata_t *) i - 1;

	CU_ASSERT_TRUE_FATAL(is_data_size(*md));
	CU_ASSERT_FALSE_FATAL(get_visitation_bit(*md) == 0);

	CU_ASSERT_EQUAL(get_data_size(*md), 8);
	CU_ASSERT_EQUAL(*i, 76543);
	CU_ASSERT_TRUE(has_room(p, 104));
	CU_ASSERT_FALSE(has_room(p, 105));

	int *j = (int *)page_alloc_data(p, 8, 1);

	CU_ASSERT_PTR_NOT_NULL(i);

	*j = 456789;
	md = (metadata_t *) i - 1;

	CU_ASSERT_TRUE_FATAL(is_data_size(*md));
	CU_ASSERT_FALSE_FATAL(get_visitation_bit(*md) == 0);

	CU_ASSERT_EQUAL(get_data_size(*md), 8);

	CU_ASSERT_EQUAL(*j, 456789);
	CU_ASSERT_TRUE(has_room(p, 88));
	CU_ASSERT_FALSE(has_room(p, 89));

	page_delete(p);
	free(memory_block);
}

void test_make_passive_reset()
{
	void *memory_block = calloc(128, 1);
	page_t *p = page_init(128, memory_block);
	make_active(p);

	int *i = (int *)page_alloc_data(p, 8, 1);
	*i = 76543;
	int *j = (int *)page_alloc_data(p, 8, 0);
	*j = 456789;

	CU_ASSERT_EQUAL(avail_space_page(p), 96);
	make_passive(p);

	CU_ASSERT_EQUAL(avail_space_page(p), 128);

	CU_ASSERT_FALSE(is_ptr_to_page(p, i));
	CU_ASSERT_FALSE(is_ptr_to_page(p, j));

	page_delete(p);
	free(memory_block);
}

void test_page_alloc_struct()
{
	void *memory_block = calloc(128, 1);
	page_t *p = page_init(128, memory_block);
	make_active(p);

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
	};

	bool actual_format_vector1[] = {0, 0, 1};

	struct s1 *ss1 = page_alloc_struct(p, actual_format_vector1, 3, 24, 0);
	CU_ASSERT_TRUE(has_room(p, 88));
	CU_ASSERT_FALSE(has_room(p, 89));

	ss1->i = 5;
	ss1->j = 987654;
	ss1->c = 'h';
	ss1->ptr = calloc(1, sizeof(int));

	*ss1->ptr = 45678;

	CU_ASSERT_EQUAL(ss1->i, 5);
	CU_ASSERT_EQUAL(ss1->j, 987654);
	CU_ASSERT_EQUAL(ss1->c, 'h');
	CU_ASSERT_EQUAL(*ss1->ptr, 45678);
	free(ss1->ptr);

	metadata_t *md1 = (metadata_t *)ss1 - 1;

	int len1 = get_size_format_vector(*md1);

	CU_ASSERT_TRUE_FATAL(is_format_vector(*md1));
	CU_ASSERT_FALSE_FATAL(get_visitation_bit(*md1) == 1);

	CU_ASSERT_EQUAL_FATAL(len1, 3);

	for (size_t i = 0; i < 3; i++)
	{
		CU_ASSERT_EQUAL(get_format_vector_idx(*md1, i), actual_format_vector1[i]);
	}

	bool actual_format_vector2[] = {0, 1, 0};

	struct s2 *ss2 = page_alloc_struct(p, actual_format_vector2, 3, 24, 1);
	CU_ASSERT_TRUE(has_room(p, 56));
	CU_ASSERT_FALSE(has_room(p, 57));

	ss2->c = 'y';
	ss2->i = 987654;
	ss2->ptr = calloc(1, sizeof(int));
	;
	ss2->f = 5.765;

	*ss2->ptr = 45678;

	CU_ASSERT_EQUAL(ss2->i, 987654);
	CU_ASSERT_EQUAL(ss2->c, 'y');
	CU_ASSERT_EQUAL(*ss2->ptr, 45678);
	CU_ASSERT_DOUBLE_EQUAL(ss2->f, 5.765, 0.0001);
	free(ss2->ptr);

	metadata_t *md2 = (metadata_t *)ss2 - 1;

	int len2 = get_size_format_vector(*md2);

	CU_ASSERT_TRUE_FATAL(is_format_vector(*md2));
	CU_ASSERT_FALSE_FATAL(get_visitation_bit(*md2) == 0);

	CU_ASSERT_EQUAL_FATAL(len2, 3);

	for (size_t i = 0; i < 3; i++)
	{
		CU_ASSERT_EQUAL(get_format_vector_idx(*md2, i), actual_format_vector2[i]);
	}

	page_delete(p);
	free(memory_block);
}

void test_is_ptr_to_page()
{
	void *memory_block = calloc(128, 1);
	page_t *p = page_init(128, memory_block);
	make_active(p);
	int *i = (int *)page_alloc_data(p, 8, 0);
	int *j = (int *)page_alloc_data(p, 8, 1);
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
	};
	bool actual_format_vector1[] = {0, 0, 1};
	bool actual_format_vector2[] = {0, 1, 0};
	struct s1 *ss1 = page_alloc_struct(p, actual_format_vector1, 3, 24, 1);
	struct s2 *ss2 = page_alloc_struct(p, actual_format_vector2, 3, 24, 1);

	int m = 5;
	int n = 7;

	CU_ASSERT_TRUE(is_ptr_to_page(p, i));
	CU_ASSERT_TRUE(is_ptr_to_page(p, j));
	CU_ASSERT_TRUE(is_ptr_to_page(p, ss1));
	CU_ASSERT_TRUE(is_ptr_to_page(p, ss2));

	CU_ASSERT_FALSE(is_ptr_to_page(p, &m));
	CU_ASSERT_FALSE(is_ptr_to_page(p, &n));
	CU_ASSERT_FALSE(is_ptr_to_page(p, NULL));

	page_delete(p);
	free(memory_block);
}

void test_avail_used_space()
{
	void *memory_block = calloc(256, 1);
	page_t *p = page_init(256, memory_block);

	CU_ASSERT_EQUAL(avail_space_page(p), 256);
	CU_ASSERT_EQUAL(used_space_page(p), 0);

	page_alloc_data(p, 64, 0);

	CU_ASSERT_EQUAL(avail_space_page(p), 184);
	CU_ASSERT_EQUAL(used_space_page(p), 72);

	bool fv[3] = {0, 0, 0};

	page_alloc_struct(p, fv, 3, 32, 0);

	CU_ASSERT_EQUAL(avail_space_page(p), 144);
	CU_ASSERT_EQUAL(used_space_page(p), 112);

	page_alloc_data(p, 136, 1);

	CU_ASSERT_EQUAL(avail_space_page(p), 0);
	CU_ASSERT_EQUAL(used_space_page(p), 256);

	page_delete(p);
	free(memory_block);
}

void test_get_actual_size()
{
	void *memory_block = calloc(256, 1);
	page_t *p = page_init(256, memory_block);

	CU_ASSERT_EQUAL(get_page_actual_size(p), sizeof(page_t) + 256 + 4);

	page_delete(p);
	free(memory_block);
}

int main()
{
	// First we try to set up CUnit, and exit if we fail
	if (CU_initialize_registry() != CUE_SUCCESS)
		return CU_get_error();

	// We then create an empty test suite and specify the name and
	// the init and cleanup functions
	CU_pSuite my_test_suite = CU_add_suite("Tests for page functions", init_suite, clean_suite);
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

	if ((CU_add_test(my_test_suite, "Test for create and destroy", test_create_destroy) == NULL) ||
		(CU_add_test(my_test_suite, "Test for make active and passive and is active", test_is_make_active_passive) == NULL) ||
		(CU_add_test(my_test_suite, "Test for has_room", test_has_room) == NULL) ||
		(CU_add_test(my_test_suite, "Test for page_alloc_data", test_page_alloc_data) == NULL) ||
		(CU_add_test(my_test_suite, "Test for make_passives reset", test_make_passive_reset) == NULL) ||
		(CU_add_test(my_test_suite, "Test for page_alloc_struct", test_page_alloc_struct) == NULL) ||
		(CU_add_test(my_test_suite, "Test for is_ptr_to_page", test_is_ptr_to_page) == NULL) ||
		(CU_add_test(my_test_suite, "Test for avail_space and used_space", test_avail_used_space) == NULL) ||
		(CU_add_test(my_test_suite, "Test for actual size", test_get_actual_size) == NULL) ||

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
