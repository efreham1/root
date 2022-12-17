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

  internal_heap_t *internal_h = h_init_internal(2, 2048);

  CU_ASSERT_PTR_NOT_NULL(internal_h);
  
  h_delete_internal(internal_h);
}


void test_h_alloc_struct_internal(void)
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
	};
	

	internal_heap_t *h = h_init_internal(2, 2048);

	struct person *pers = h_alloc_struct_internal(h, "**ild");
	CU_ASSERT_PTR_NOT_NULL(pers);
	pers->age = 69;
	pers->first_name = "Lennart";
	pers->last_name = "Åkesson";
	pers->prs_num = 5308091327;
	pers->d = 556.234567;
	CU_ASSERT_EQUAL(pers->age, 69);
	CU_ASSERT_STRING_EQUAL(pers->first_name, "Lennart");
	CU_ASSERT_STRING_EQUAL(pers->last_name, "Åkesson");
	CU_ASSERT_EQUAL(pers->prs_num, 5308091327);
	CU_ASSERT_DOUBLE_EQUAL(pers->d, 556.234567, 0.0001);

	struct house *hus = h_alloc_struct_internal(h, "i**il");
	CU_ASSERT_PTR_NOT_NULL(hus);
	hus->city = "Åtvidaberg";
	hus->num_windows = 1;
	hus->postcode = 20000;
	hus->price_in_ore = 7000000000;
	hus->wall_colour = "Orange";
	CU_ASSERT_EQUAL(hus->num_windows, 1);
	CU_ASSERT_EQUAL(hus->postcode, 20000);
	CU_ASSERT_EQUAL(hus->price_in_ore, 7000000000);
	CU_ASSERT_STRING_EQUAL(hus->city, "Åtvidaberg");
	CU_ASSERT_STRING_EQUAL(hus->wall_colour, "Orange");

	h_delete_internal(h);
}

void test_h_alloc_data_internal(void)
{
	internal_heap_t *h = h_init_internal(2, 2048);

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

void test_is_valid_ptr()
{
	struct person
	{
		char *first_name;
		char *last_name;
		int age;
		long prs_num;
	};

	struct house
	{
		int num_windows;
		char *city;
		char *wall_colour;
		int postcode;
		long price_in_ore;
	};

	internal_heap_t *h = h_init_internal(2, 2048);

	int *data = h_alloc_data_internal(h, sizeof(int));
	int *data2 = h_alloc_data_internal(h, sizeof(int));
	struct house *hus = h_alloc_struct_internal(h, "i**il");
	struct person *pers = h_alloc_struct_internal(h, "**il");

	int i = 5;
	char *str = "nujhn345uyiwghs";

	CU_ASSERT_TRUE(is_valid_ptr(h, data));
	CU_ASSERT_TRUE(is_valid_ptr(h, data2));
	CU_ASSERT_TRUE(is_valid_ptr(h, hus));
	CU_ASSERT_TRUE(is_valid_ptr(h, pers));

	CU_ASSERT_FALSE(is_valid_ptr(h, &i));
	CU_ASSERT_FALSE(is_valid_ptr(h, str));
	CU_ASSERT_FALSE(is_valid_ptr(h, NULL));

	h_delete_internal(h);
}

int main()
{
	// First we try to set up CUnit, and exit if we fail
	if (CU_initialize_registry() != CUE_SUCCESS)
		return CU_get_error();

	// We then create an empty test suite and specify the name and
	// the init and cleanup functions
	CU_pSuite my_test_suite = CU_add_suite("Tests for heap functions", init_suite, clean_suite);
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
		(CU_add_test(my_test_suite, "Test for validating pointer", test_is_valid_ptr) == NULL) ||

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
