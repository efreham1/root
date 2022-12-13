#include <CUnit/Basic.h>
#include <stdlib.h>
#include "page.h"
#include "metadata.h"

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
	page_t *p = page_init(345);

	CU_ASSERT_PTR_NOT_NULL(p);

	page_delete(p);
}

void test_is_make_active()
{
	page_t *p = page_init(345);

	CU_ASSERT_FALSE(is_active(p));

	make_active(p);

	CU_ASSERT_TRUE(is_active(p));

	page_delete(p);
}

void test_has_room()
{
	page_t *p = page_init(64);

	make_active(p);

	CU_ASSERT_TRUE(has_room(p, 56));
	CU_ASSERT_FALSE(has_room(p, 57));

	page_delete(p);
}

void test_page_alloc_data()
{
	page_t *p = page_init(128);
	make_active(p);
	CU_ASSERT_TRUE(has_room(p, 120));
	CU_ASSERT_FALSE(has_room(p, 121));
	int *i = (int *)page_alloc_data(p, 8);
	CU_ASSERT_PTR_NOT_NULL(i);
	*i = 76543;
	CU_ASSERT_EQUAL(*i, 76543);

	CU_ASSERT_TRUE(has_room(p, 104));
	CU_ASSERT_FALSE(has_room(p, 105));

	int *j = (int *)page_alloc_data(p, 8);
	CU_ASSERT_PTR_NOT_NULL(i);
	*j = 456789;
	CU_ASSERT_EQUAL(*j, 456789);

	CU_ASSERT_TRUE(has_room(p, 88));
	CU_ASSERT_FALSE(has_room(p, 89));

	page_delete(p);
}

void test_page_alloc_struct()
{
	page_t *p = page_init(128);
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

	struct s1 *ss1 = page_alloc_struct(p, actual_format_vector1, 3, 24);
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

	metadata_t *md1 = (metadata_t *) ss1 - 1;

	int len1 = 0;

	CU_ASSERT_TRUE_FATAL(is_format_vector(*md1));

	bool *format_vector1 = get_format_vector(*md1, &len1);

	CU_ASSERT_EQUAL_FATAL(len1, 3);

	for (size_t i = 0; i < 3; i++)
	{
		CU_ASSERT_EQUAL(format_vector1[i], actual_format_vector1[i]);
	}
	
	free(format_vector1);

	bool actual_format_vector2[] = {0, 1, 0};

	struct s2 *ss2 = page_alloc_struct(p, actual_format_vector2, 3, 24);
	CU_ASSERT_TRUE(has_room(p, 56));
	CU_ASSERT_FALSE(has_room(p, 57));

	ss2->c = 'y';
	ss2->i = 987654;
	ss2->ptr = calloc(1, sizeof(int));;
	ss2->f = 5.765000;

	*ss2->ptr = 45678;

	CU_ASSERT_EQUAL(ss2->i, 987654);
	CU_ASSERT_EQUAL(ss2->c, 'y');
	CU_ASSERT_EQUAL(*ss2->ptr, 45678);
	free(ss2->ptr);

	metadata_t *md2 = (metadata_t *) ss2 - 1;

	int len2 = 0;

	CU_ASSERT_TRUE_FATAL(is_format_vector(*md2));

	bool *format_vector2 = get_format_vector(*md2, &len2);

	CU_ASSERT_EQUAL_FATAL(len2, 3);

	for (size_t i = 0; i < 3; i++)
	{
		CU_ASSERT_EQUAL(format_vector2[i], actual_format_vector2[i]);
	}

	free(format_vector2);

	page_delete(p);
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
		(CU_add_test(my_test_suite, "Test for make and is active", test_is_make_active) == NULL) ||
		(CU_add_test(my_test_suite, "Test for has_room", test_has_room) == NULL) ||
		(CU_add_test(my_test_suite, "Test for page_alloc_data", test_page_alloc_data) == NULL) ||
		(CU_add_test(my_test_suite, "Test for page_alloc_struct", test_page_alloc_struct) == NULL) ||

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
