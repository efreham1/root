#include <CUnit/Basic.h>
#include <stdlib.h>
#include "heap.h"
#include <stdbool.h>


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

  internal_heap_t *internal_h = h_init_internal(4500, 2048);

  CU_ASSERT_PTR_NOT_NULL(internal_h);
  
  h_delete_internal(internal_h);
}


void test_h_alloc_struct_internal(void)
{
	CU_ASSERT_TRUE(false); //TODO
}

void test_h_alloc_data_internal(void)
{
	internal_heap_t *h = h_init_internal(120, 2048);

	int *data = h_alloc_data_internal(h, sizeof(int));
	CU_ASSERT_PTR_NOT_NULL(data);
	*data = 5;
	CU_ASSERT_TRUE(*data == 5);

	int *data2 = h_alloc_data_internal(h, sizeof(int));
	CU_ASSERT_PTR_NOT_NULL(data2);
	*data2 = 456789;
	CU_ASSERT_TRUE(*data2 == 456789);

	h_delete_internal(h);

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

    if ((CU_add_test(my_test_suite, "Test for create and destroy",test_create_destroy) == NULL) ||
		(CU_add_test(my_test_suite, "Test for allocating a struct", test_h_alloc_struct_internal) == NULL) ||
		(CU_add_test(my_test_suite, "Test for allocating raw data", test_h_alloc_data_internal) == NULL) ||

        0
    )

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
