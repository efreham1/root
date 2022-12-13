#include <CUnit/Basic.h>
#include <stdlib.h>
#include "gc.h"
#include "input_handler.h"


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
  heap_t *h = h_init(2,false,5.0);

  CU_ASSERT_PTR_NOT_NULL(h);
  
  h_delete(h);

}

void test_h_delete_dbg(void)
{
  heap_t *h = h_init(2,false,5.0);

  CU_ASSERT_PTR_NOT_NULL_FATAL(h);

  void *ptr_data = h_alloc_data(h,8);
  void *ptr_struct = h_alloc_struct(h,"i2*f3c");

  CU_ASSERT_PTR_NOT_NULL(ptr_data);
  CU_ASSERT_PTR_NOT_NULL(ptr_struct);

  h_delete_dbg(h,NULL);

  CU_ASSERT_PTR_NULL(ptr_data);
  CU_ASSERT_PTR_NULL(ptr_struct);
}

void test_alloc_struct(void)
{
	heap_t *h = h_init(2, false, 5.0);

	// h_alloc_struct(h, )
	CU_ASSERT_TRUE(false);

	h_delete(h);
}

void test_alloc_data(void)
{
	heap_t *h = h_init(2, false, 5.0);

	int *data = h_alloc_data(h, sizeof(int));

	*data = 5;

	CU_ASSERT_PTR_NOT_NULL(data);
	CU_ASSERT_TRUE(*data == 5);

	h_delete(h);
}


void test_h_gc(void) //run a gc
{
  CU_ASSERT_TRUE(false);
}

void test_h_gc_dbg(void) // run a gc in which stack pointers are safe (?)
{
  CU_ASSERT_TRUE(false);
}

void test_h_avail(void) // return availible free memory (as an unsigned int)
{
	CU_ASSERT_TRUE(false);
}

void test_h_used(void) //return used data (as an unsigned int)
{
	CU_ASSERT_TRUE(false);
}

void test_input_handler(void)
{
	char *s1 = handle_input("10i2l**4*1c");
	char *s2 = handle_input("gul lök");
	char *s3 = handle_input("2*0L");
	char *s4 =  handle_input("**ll2ld3d4d");
	char *s5 = handle_input("");

	CU_ASSERT_STRING_EQUAL("iiiiiiiiiill******c", s1);

	CU_ASSERT_PTR_EQUAL((char *) 0xDEADCAFEBABEBEEF , s2);
	
	CU_ASSERT_PTR_EQUAL((char *) 0xDEADCAFEBABEBEEF , s3);

	CU_ASSERT_STRING_EQUAL("**lllldddddddd", s4);

	CU_ASSERT_PTR_EQUAL((char *) 0xDEADCAFEBABEBEEF , s5);

	free(s1);
	free(s4);

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
	(CU_add_test(my_test_suite, "Test for destroy with DBG",test_h_delete_dbg) == NULL) ||
	(CU_add_test(my_test_suite, "Test for alloc of struct",test_alloc_struct) == NULL) ||
	(CU_add_test(my_test_suite, "Test for alloc of data",test_alloc_data) == NULL) ||
	(CU_add_test(my_test_suite, "Test for ",test_h_gc) == NULL) ||
	(CU_add_test(my_test_suite, "Test for ..",test_h_gc_dbg) == NULL) ||
	(CU_add_test(my_test_suite, "Test for ..",test_h_avail) == NULL) ||
	(CU_add_test(my_test_suite, "Test for ..",test_h_used) == NULL) ||
	(CU_add_test(my_test_suite, "Test for the input handler",test_input_handler) == NULL) ||

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
