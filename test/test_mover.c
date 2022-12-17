#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
#include "mover.h"
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

void test_do_move()
{
	void *memory_block = calloc(256, 1);
	page_t *active_page = page_init(128, memory_block);
	page_t *passive_page = page_init(128, memory_block + 128);

	make_active(active_page);

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

	struct s2 *ss2 = page_alloc_struct(active_page, format_vector2, 4, 32);

	ss2->c = 'y';
	ss2->i = 987654;
	ss2->ptr = page_alloc_data(active_page, 8);
	ss2->f = 5.765000;

	*ss2->ptr = 45678;

	bool format_vector1[] = {0, 0, 1};

	ss2->linked_struct = page_alloc_struct(active_page, format_vector1, 3, 24);

	ss2->linked_struct->i = 5;
	ss2->linked_struct->j = 987654;
	ss2->linked_struct->c = 'h';
	ss2->linked_struct->ptr = page_alloc_data(active_page, 8);

	*ss2->linked_struct->ptr = 45678;

	CU_ASSERT_FALSE(is_active(passive_page));

	CU_ASSERT_FALSE(is_ptr_to_page(passive_page, ss2));
	CU_ASSERT_TRUE(is_ptr_to_page(active_page, ss2));

	CU_ASSERT_TRUE(ss2->c == 'y');
	CU_ASSERT_TRUE(ss2->i == 987654);
	CU_ASSERT_TRUE(*ss2->ptr == 45678);

	CU_ASSERT_FALSE(is_ptr_to_page(passive_page, ss2->linked_struct));
	CU_ASSERT_TRUE(is_ptr_to_page(active_page, ss2->linked_struct));

	CU_ASSERT_TRUE(ss2->linked_struct->c == 'h');
	CU_ASSERT_TRUE(ss2->linked_struct->i == 5);
	CU_ASSERT_TRUE(ss2->linked_struct->j == 987654);
	CU_ASSERT_TRUE(*ss2->linked_struct->ptr == 45678);

	CU_ASSERT_FALSE(is_ptr_to_page(passive_page, ss2->ptr));
	CU_ASSERT_TRUE(is_ptr_to_page(active_page, ss2->ptr));

	CU_ASSERT_FALSE(is_ptr_to_page(passive_page, ss2->linked_struct->ptr));
	CU_ASSERT_TRUE(is_ptr_to_page(active_page, ss2->linked_struct->ptr));

	do_move((void **)&ss2, &passive_page, 1, NULL, 0);

	CU_ASSERT_TRUE(is_active(passive_page));

	CU_ASSERT_TRUE(is_ptr_to_page(passive_page, ss2));
	CU_ASSERT_FALSE(is_ptr_to_page(active_page, ss2));

	CU_ASSERT_TRUE(ss2->c == 'y');
	CU_ASSERT_TRUE(ss2->i == 987654);
	CU_ASSERT_TRUE(*ss2->ptr == 45678);

	CU_ASSERT_TRUE(is_ptr_to_page(passive_page, ss2->linked_struct));
	CU_ASSERT_FALSE(is_ptr_to_page(active_page, ss2->linked_struct));

	CU_ASSERT_TRUE(ss2->linked_struct->c == 'h');
	CU_ASSERT_TRUE(ss2->linked_struct->i == 5);
	CU_ASSERT_TRUE(ss2->linked_struct->j == 987654);
	CU_ASSERT_TRUE(*ss2->linked_struct->ptr == 45678);

	CU_ASSERT_TRUE(is_ptr_to_page(passive_page, ss2->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(active_page, ss2->ptr));

	CU_ASSERT_TRUE(is_ptr_to_page(passive_page, ss2->linked_struct->ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(active_page, ss2->linked_struct->ptr));

	page_delete(active_page);
	page_delete(passive_page);
	free(memory_block);
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

	void *memory_block = calloc(256, 1);
	page_t *active_page = page_init(128, memory_block);
	page_t *passive_page = page_init(128, memory_block + 128);

	make_active(active_page);
	
	bool format_vector[] = {1, 0};

	struct s1 *ss1 = page_alloc_struct(active_page, format_vector, 2, 16);

	ss1->i = 567;
	ss1->s_ptr = page_alloc_struct(active_page, format_vector, 2, 16);

	ss1->s_ptr->i = 987;
	ss1->s_ptr->s_ptr = page_alloc_struct(active_page, format_vector, 2, 16);

	ss1->s_ptr->s_ptr->i = 2345;
	ss1->s_ptr->s_ptr->s_ptr = ss1;

	CU_ASSERT_FALSE(is_active(passive_page));

	CU_ASSERT_TRUE(is_ptr_to_page(active_page, ss1));
	CU_ASSERT_FALSE(is_ptr_to_page(passive_page, ss1));

	CU_ASSERT_TRUE(is_ptr_to_page(active_page, ss1->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(passive_page, ss1->s_ptr));

	CU_ASSERT_TRUE(is_ptr_to_page(active_page, ss1->s_ptr->s_ptr));
	CU_ASSERT_FALSE(is_ptr_to_page(passive_page, ss1->s_ptr->s_ptr));

	CU_ASSERT_PTR_EQUAL(ss1->s_ptr->s_ptr->s_ptr, ss1);

	do_move((void **) &ss1, &passive_page, 1, NULL, 0);

	CU_ASSERT_TRUE(is_active(passive_page));

	CU_ASSERT_FALSE(is_ptr_to_page(active_page, ss1));
	CU_ASSERT_TRUE(is_ptr_to_page(passive_page, ss1));
	CU_ASSERT_EQUAL(ss1->i, 567);

	CU_ASSERT_FALSE(is_ptr_to_page(active_page, ss1->s_ptr));
	CU_ASSERT_TRUE(is_ptr_to_page(passive_page, ss1->s_ptr));
	CU_ASSERT_EQUAL(ss1->s_ptr->i, 987);

	CU_ASSERT_FALSE(is_ptr_to_page(active_page, ss1->s_ptr->s_ptr));
	CU_ASSERT_TRUE(is_ptr_to_page(passive_page, ss1->s_ptr->s_ptr));
	CU_ASSERT_EQUAL(ss1->s_ptr->s_ptr->i, 2345);

	CU_ASSERT_PTR_EQUAL(ss1->s_ptr->s_ptr->s_ptr, ss1);

	page_delete(active_page);
	page_delete(passive_page);
	free(memory_block);
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

	void *memory_block = calloc(512, 1);
	page_t *active_page1 = page_init(128, memory_block);
	page_t *active_page2 = page_init(128, memory_block + 128);
	page_t *passive_page1 = page_init(128, memory_block + 256);
	page_t *passive_page2 = page_init(128, memory_block + 384);

	page_t *passive_pages[] = {passive_page1, passive_page2};

	make_active(active_page1);
	make_active(active_page2);

	bool format_vector[] = {1, 0, 1};

	struct s1 *ss1 = page_alloc_struct(active_page1, format_vector, 3, 24);
	ss1->i = 76;
	ss1->ptr = page_alloc_data(passive_page1, 8);
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
