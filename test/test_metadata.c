#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
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

void test_is_format_vector()
{
	metadata_t md = (void *) 3;
	CU_ASSERT_TRUE(is_format_vector(md));
	CU_ASSERT_FALSE(is_data_size(md) && is_forward_address(md));
}

void test_is_data_size()
{
	metadata_t md = (void *) 1;
	CU_ASSERT_TRUE(is_data_size(md));
	CU_ASSERT_FALSE(is_format_vector(md) && is_forward_address(md));
}

void test_is_forward_address()
{
	metadata_t md = malloc(1);
	CU_ASSERT_TRUE(is_forward_address(md));
	CU_ASSERT_FALSE(is_data_size(md) && is_format_vector(md));
	free(md);
}

void test_set_get_forward_address()
{
	int *i = malloc(sizeof(int));
	*i = 42;
	metadata_t md = set_forward_address(i);
	int *j = (int *) get_forward_address(md);
	CU_ASSERT_PTR_EQUAL(j, i);
	CU_ASSERT_EQUAL(*i, *j);
	free(i);
}

void test_set_get_data_size()
{
	unsigned int size = 8765432;
	metadata_t md = set_data_size(size);
	unsigned int gotten_size = get_data_size(md);
	CU_ASSERT_EQUAL(gotten_size, size);
}

void test_set_get_format_vector()
{
	bool format_vector[13] = {0,1,0,0,0,1,1,0,1,0,0,1,1};
	metadata_t md = set_format_vector(format_vector, 13);
	int len = 0;
	bool *gotten_format_vector = get_format_vector(md, &len);
	for (unsigned int i = 0; i < 13; i++)
	{
		CU_ASSERT_EQUAL(format_vector[i], gotten_format_vector[i]);
	}
	free(gotten_format_vector);
}

void test_get_size_struct()
{
	bool format_vector[13] = {0,1,0,0,0,1,1,0,1,0,0,1,1};
	metadata_t md = set_format_vector(format_vector, 13);
	CU_ASSERT_TRUE(get_size_struct(md) == 13*8);

	bool format_vector2[0] = {};
	metadata_t md2 = set_format_vector(format_vector2, 0);
	CU_ASSERT_TRUE(get_size_struct(md2) == 0);
	
	bool format_vector3[1] = {0};
	metadata_t md3 = set_format_vector(format_vector3, 1);
	CU_ASSERT_TRUE(get_size_struct(md3) == 1*8);
	
}

int main()
{
	// First we try to set up CUnit, and exit if we fail
	if (CU_initialize_registry() != CUE_SUCCESS)
		return CU_get_error();

	// We then create an empty test suite and specify the name and
	// the init and cleanup functions
	CU_pSuite my_test_suite = CU_add_suite("Tests for metadata functions", init_suite, clean_suite);
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

    if ((CU_add_test(my_test_suite, "Test for is_format_vector", test_is_format_vector) == NULL) ||
		(CU_add_test(my_test_suite, "Test for is_data_size",  test_is_data_size) == NULL) ||
		(CU_add_test(my_test_suite, "Test for is_forward_address",  test_is_forward_address) == NULL) ||
		(CU_add_test(my_test_suite, "Test for set and get forward_address",  test_set_get_forward_address) == NULL) ||
		(CU_add_test(my_test_suite, "Test for set and get data_size",  test_set_get_data_size) == NULL) ||
		(CU_add_test(my_test_suite, "Test for set and get format_vector",  test_set_get_format_vector) == NULL) ||
		(CU_add_test(my_test_suite, "Test for metadata size",  test_get_size_struct) == NULL) ||

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
