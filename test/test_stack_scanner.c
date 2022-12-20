#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack_scanner.h"

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

void test_stack_scanner()
{
    int *i = calloc(5, sizeof(int));
    int *j = calloc(1, sizeof(int));
	int is[] = {1, 2, 3, 4, 5};
    int *k = calloc(1, sizeof(int));
    int *m = calloc(1, sizeof(int));

    CU_ASSERT_FATAL(i < j && j < k && k < m);

    int len = 0;
    void ***stack_ptrs = stack_addresses((void *) m, (void *) i, &len);

    CU_ASSERT_EQUAL(len, 4);
    CU_ASSERT_PTR_EQUAL(*stack_ptrs[0], i);
    CU_ASSERT_PTR_EQUAL(*stack_ptrs[1], j);
    CU_ASSERT_PTR_EQUAL(*stack_ptrs[2], k);
    CU_ASSERT_PTR_EQUAL(*stack_ptrs[3], m);

	for (size_t idx = 0; idx < 4; idx++)
	{
		CU_ASSERT_PTR_NOT_EQUAL(*stack_ptrs[idx], is);
	}
	
	

    free(i);
    free(j);
    free(k);
    free(m);
    free(stack_ptrs);
}

int main()
{
	// First we try to set up CUnit, and exit if we fail
	if (CU_initialize_registry() != CUE_SUCCESS)
		return CU_get_error();

	// We then create an empty test suite and specify the name and
	// the init and cleanup functions
	CU_pSuite my_test_suite = CU_add_suite("Tests for stack scanner functions", init_suite, clean_suite);
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

	if ((CU_add_test(my_test_suite, "Test for finding pointers on the stack", test_stack_scanner) == NULL) ||

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
