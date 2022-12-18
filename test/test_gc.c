#include <CUnit/Basic.h>
#include <stdlib.h>
#include "gc.h"
#include "input_handler.h"
#include "heap.h"
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

void test_create_destroy(void)
{
	heap_t *h = h_init(10, false, 0.5);

	CU_ASSERT_PTR_NOT_NULL(h);

	h_delete(h);
}

void test_h_delete_dbg(void)
{
	heap_t *h = h_init(32, false, 0.5);

	CU_ASSERT_PTR_NOT_NULL_FATAL(h);

	void *ptr_data = h_alloc_data(h, 8);
	void *ptr_struct = h_alloc_struct(h, "i2*f3c");

	CU_ASSERT_PTR_NOT_NULL(ptr_data);
	CU_ASSERT_PTR_NOT_NULL(ptr_struct);

	h_delete_dbg(h, NULL);

	CU_ASSERT_PTR_NULL(ptr_data);
	CU_ASSERT_PTR_NULL(ptr_struct);
}

void test_alloc_struct(void)
{
	struct s1
	{
		char c1;
		char c2;
		char c3;
		char c4;
		char c5;
		int i;
		float *ptr;
	};

	heap_t *h = h_init(200, false, 0.5);

	float bruh = 5.674;

	struct s1 *ss1 = h_alloc_struct(h, "5ci*");

	ss1->c1 = 'a';
	ss1->c2 = 'b';
	ss1->c3 = 'c';
	ss1->c4 = 'd';
	ss1->c5 = 'e';
	ss1->i = 675;
	ss1->ptr = &bruh;

	CU_ASSERT_EQUAL(ss1->c1, 'a');
	CU_ASSERT_EQUAL(ss1->c2, 'b');
	CU_ASSERT_EQUAL(ss1->c3, 'c');
	CU_ASSERT_EQUAL(ss1->c4, 'd');
	CU_ASSERT_EQUAL(ss1->c5, 'e');
	CU_ASSERT_EQUAL(ss1->i, 675);
	CU_ASSERT_PTR_EQUAL(ss1->ptr, &bruh);

	h_delete(h);
}

void test_alloc_data(void)
{
	heap_t *h = h_init(200, false, 0.5);

	int *data = h_alloc_data(h, sizeof(int));

	*data = 5;

	CU_ASSERT_PTR_NOT_NULL(data);
	CU_ASSERT_TRUE(*data == 5);

	h_delete(h);
}

void test_h_gc(void)
{
	heap_t *h = h_init(6000, false, 0.5);

	int *data_1 = h_alloc_data(h, sizeof(int));

	CU_ASSERT_PTR_NOT_NULL(data_1);

	data_1 = NULL;

	unsigned int bytes_collected = h_gc(h);

	CU_ASSERT_FALSE(0 == bytes_collected);

	h_delete(h);
}

void test_h_gc_dbg(void)
{
	heap_t *h = h_init(6000, false, 0.5);

	int *data_1 = h_alloc_data(h, sizeof(int));
	int *data_2 = h_alloc_data(h, sizeof(int));

	data_1 = NULL;

	unsigned int bytes_collected = h_gc(h);

	CU_ASSERT_FALSE(0 == bytes_collected);
	
	data_1 = h_alloc_data(h, sizeof(int));

	CU_ASSERT_PTR_NOT_NULL(data_1);

	data_1 = NULL;

	bytes_collected = h_gc_dbg(h, true);

	CU_ASSERT_TRUE(0 == bytes_collected);
	CU_ASSERT_PTR_NOT_NULL(data_2);

	h_delete(h);
}

void test_h_avail(void)
{
	heap_t *h = h_init(6000, false, 0.5);

	CU_ASSERT_EQUAL(h_avail(h), 6144);

	h_delete(h);
}

void test_h_used(void) // return used data (as an unsigned int)
{
	heap_t *h = h_init(6000, false, 0.5);

	CU_ASSERT_EQUAL(h_used(h), 0);

	h_delete(h);
}

void test_input_handler(void)
{
	char *s1 = handle_input("10i2l**4*1c");
	char *s2 = handle_input("gul lök");
	char *s3 = handle_input("2*0L");
	char *s4 = handle_input("**ll2ld3d4d");
	char *s5 = handle_input("");

	CU_ASSERT_STRING_EQUAL("iiiiiiiiiill******c", s1);

	CU_ASSERT_PTR_EQUAL((char *)0xDEADCAFEBABEBEEF, s2);

	CU_ASSERT_PTR_EQUAL((char *)0xDEADCAFEBABEBEEF, s3);

	CU_ASSERT_STRING_EQUAL("**lllldddddddd", s4);

	CU_ASSERT_PTR_EQUAL((char *)0xDEADCAFEBABEBEEF, s5);

	free(s1);
	free(s4);
}

void test_get_actual_size()
{
	heap_t *h = h_init(6000, false, 0.5);

	CU_ASSERT_EQUAL(get_heap_actual_size(h), get_internal_heap_actual_size(h->internal_heap) + sizeof(heap_t));

	h_delete(h);
}

int main()
{
	// First we try to set up CUnit, and exit if we fail
	if (CU_initialize_registry() != CUE_SUCCESS)
		return CU_get_error();

	// We then create an empty test suite and specify the name and
	// the init and cleanup functions
	CU_pSuite my_test_suite = CU_add_suite("Tests for gc functions", init_suite, clean_suite);
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
		//(CU_add_test(my_test_suite, "Test for destroy with DBG", test_h_delete_dbg) == NULL) ||
		(CU_add_test(my_test_suite, "Test for alloc of struct", test_alloc_struct) == NULL) ||
		(CU_add_test(my_test_suite, "Test for alloc of data", test_alloc_data) == NULL) ||
		//(CU_add_test(my_test_suite, "Test for manual gc", test_h_gc) == NULL) ||
		//(CU_add_test(my_test_suite, "Test for manual gc_dbg", test_h_gc_dbg) == NULL) ||
		(CU_add_test(my_test_suite, "Test for available space", test_h_avail) == NULL) ||
		(CU_add_test(my_test_suite, "Test for used space", test_h_used) == NULL) ||
		(CU_add_test(my_test_suite, "Test for the input handler", test_input_handler) == NULL) ||
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
