#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack_scanner.h"

extern char **environ;

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

void test_trash(){

	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;
	char e = 'e';
	char f = 'f';
	char g = 'g';

	void *bottom_address = (void *)environ;
    void *top_address = __builtin_frame_address(0);

    int len = 0;
    void ***stack_ptrs = stack_addresses(top_address, bottom_address, &len);
	CU_ASSERT_EQUAL(len, 0);
	CU_ASSERT_PTR_NOT_EQUAL(stack_ptrs[0], &a);
	CU_ASSERT_PTR_NOT_EQUAL(stack_ptrs[2], &c);
	CU_ASSERT_PTR_NOT_EQUAL(stack_ptrs[1], &b);
	CU_ASSERT_PTR_NOT_EQUAL(stack_ptrs[3], &d);
	CU_ASSERT_PTR_NOT_EQUAL(stack_ptrs[4], &e);
	CU_ASSERT_PTR_NOT_EQUAL(stack_ptrs[5], &f);
	CU_ASSERT_PTR_NOT_EQUAL(stack_ptrs[6], &g);

	free(stack_ptrs);
}

void test_calloc_array(){

	int *t1 = calloc(1, sizeof(int));
	int *t2 = calloc(1, sizeof(int));
	int *t3 = calloc(1, sizeof(int));
	int *t4 = calloc(1, sizeof(int));
	int *t5 = calloc(1, sizeof(int));
	int *t6 = calloc(1, sizeof(int));
	int *t7 = calloc(1, sizeof(int));

	int **array[] ={&t1,&t2,&t3,&t4,&t5,&t6,&t7};
	int length = 0;
	void ***stack = stack_addresses((void*) t7, (void*) t1, &length);
	CU_ASSERT_EQUAL(length, 7);

	for (int i = 0; i < 7; i++)
	{
		CU_ASSERT_PTR_EQUAL(*stack[i], *array[i]);
	}
	
	free(t1);
	free(t2);
	free(t3);
	free(t4);
	free(t5);
	free(t6);
	free(t7);
	free(stack);
}

void test_mix(){
	char *s1 = calloc(1, sizeof(char));
	char *s2 = calloc(1, sizeof(char));
	char *s3 = calloc(1, sizeof(char));
	int a = 1;
	int i = 10;
	int *v1 = calloc(1, sizeof(int));
	int *v2 = calloc(1, sizeof(int));

	int length = 0;
	void ***stack = stack_addresses((void*) v2, (void*) s1, &length);
	CU_ASSERT_EQUAL(length, 5);

	CU_ASSERT_PTR_EQUAL(*stack[0], s1);
	CU_ASSERT_PTR_EQUAL(*stack[1], s2);
	CU_ASSERT_PTR_EQUAL(*stack[2], s3);
	CU_ASSERT_PTR_NOT_EQUAL(*stack[3], a);
	CU_ASSERT_PTR_EQUAL(*stack[3], v1);
	CU_ASSERT_PTR_NOT_EQUAL(*stack[4], i);
	CU_ASSERT_PTR_EQUAL(*stack[4], v2);	
	
	free(s1);
	free(s2);
	free(s3);
	free(v1);
	free(v2);
	free(stack);
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
		(CU_add_test(my_test_suite, "Test for non-pointers on the stack", test_trash) == NULL) ||
		(CU_add_test(my_test_suite, "Test for finding pointers in an array", test_calloc_array) == NULL) ||
		(CU_add_test(my_test_suite, "Test for finding pointers on the stack with other garbage", test_mix) == NULL) ||
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
