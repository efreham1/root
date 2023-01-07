#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "linked_list.h"
#include "linked_list_malloc.h"
#include "gc.h"
#include "data_structure.h"
#include <stdlib.h>

bool int_eq_fun(elem_t a, elem_t b)
{
    return a.int_v == b.int_v;
}

int main(int argc, char const *argv[])
{
    FILE *f = fopen("perf_comparison/times_linked_lists.csv", "w");
    struct timespec tstart = {0, 0}, tend = {0, 0};
    srand(time(NULL));

    for (size_t i = 10000; i < 20001; i+=100)
    {
        heap_t *h = h_init(200 + i * 224, true, 1.0);
        clock_gettime(CLOCK_MONOTONIC, &tstart);

        ioopm_list_t *halloc_list1 = ioopm_linked_list_create(int_eq_fun, h);
        ioopm_list_t *halloc_list2 = ioopm_linked_list_create(int_eq_fun, h);
        ioopm_list_t *halloc_list3 = ioopm_linked_list_create(int_eq_fun, h);
        ioopm_list_t *halloc_list4 = ioopm_linked_list_create(int_eq_fun, h);

        for (size_t j = 0; j < i; j++)
        {
            int r = rand() % (4000000000);
            if (r > 1000000000)
            {
                ioopm_linked_list_append(halloc_list1, (elem_t){.int_v = r}, h);
            }
            else if (r > 2000000000)
            {
                ioopm_linked_list_append(halloc_list2, (elem_t){.int_v = r}, h);
            }
            else if (r > 3000000000)
            {
                ioopm_linked_list_append(halloc_list3, (elem_t){.int_v = r}, h);
            }
            else
            {
                ioopm_linked_list_append(halloc_list4, (elem_t){.int_v = r}, h);
            }
        }

        for (size_t j = 0; j < i / 10; j++)
        {
            int r = rand() % (4000000000);
            if (r > 1000000000)
            {
                ioopm_linked_list_contains(halloc_list1, (elem_t){.int_v = r});
            }
            else if (r > 2000000000)
            {
                ioopm_linked_list_contains(halloc_list2, (elem_t){.int_v = r});
            }
            else if (r > 3000000000)
            {
                ioopm_linked_list_contains(halloc_list3, (elem_t){.int_v = r});
            }
            else
            {
                ioopm_linked_list_contains(halloc_list4, (elem_t){.int_v = r});
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &tend);
    
        h_delete(h);

        fprintf(f, "%lu, %.5f, ", i, ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec));

        h = h_init(200 + i * 224, true, 1.0);
        clock_gettime(CLOCK_MONOTONIC, &tstart);

        halloc_list1 = ioopm_linked_list_create(int_eq_fun, h);
        halloc_list2 = ioopm_linked_list_create(int_eq_fun, h);
        halloc_list3 = ioopm_linked_list_create(int_eq_fun, h);
        halloc_list4 = ioopm_linked_list_create(int_eq_fun, h);

        for (size_t j = 0; j < i; j++)
        {
            int r = rand() % (4000000000);
            if (r > 1000000000)
            {
                ioopm_linked_list_append(halloc_list1, (elem_t){.int_v = r}, h);
            }
            else if (r > 2000000000)
            {
                ioopm_linked_list_append(halloc_list2, (elem_t){.int_v = r}, h);
            }
            else if (r > 3000000000)
            {
                ioopm_linked_list_append(halloc_list3, (elem_t){.int_v = r}, h);
            }
            else
            {
                ioopm_linked_list_append(halloc_list4, (elem_t){.int_v = r}, h);
            }
        }

        h_gc(h);

        for (size_t j = 0; j < i / 10; j++)
        {
            int r = rand() % (4000000000);
            if (r > 1000000000)
            {
                ioopm_linked_list_contains(halloc_list1, (elem_t){.int_v = r});
            }
            else if (r > 2000000000)
            {
                ioopm_linked_list_contains(halloc_list2, (elem_t){.int_v = r});
            }
            else if (r > 3000000000)
            {
                ioopm_linked_list_contains(halloc_list3, (elem_t){.int_v = r});
            }
            else
            {
                ioopm_linked_list_contains(halloc_list4, (elem_t){.int_v = r});
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &tend);
    
        h_delete(h);

        fprintf(f, "%.5f, ", ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec));

        clock_gettime(CLOCK_MONOTONIC, &tstart);

        ioopm_list_t *malloc_list1 = ioopm_linked_list_malloc_create(int_eq_fun);
        ioopm_list_t *malloc_list2 = ioopm_linked_list_malloc_create(int_eq_fun);
        ioopm_list_t *malloc_list3 = ioopm_linked_list_malloc_create(int_eq_fun);
        ioopm_list_t *malloc_list4 = ioopm_linked_list_malloc_create(int_eq_fun);

        for (size_t j = 0; j < i; j++)
        {
            int r = rand() % (4000000000);
            if (r > 1000000000)
            {
                ioopm_linked_list_malloc_append(malloc_list1, (elem_t){.int_v = r});
            }
            else if (r > 2000000000)
            {
                ioopm_linked_list_malloc_append(malloc_list2, (elem_t){.int_v = r});
            }
            else if (r > 3000000000)
            {
                ioopm_linked_list_malloc_append(malloc_list3, (elem_t){.int_v = r});
            }
            else
            {
                ioopm_linked_list_malloc_append(malloc_list4, (elem_t){.int_v = r});
            }
        }

        for (size_t j = 0; j < i / 10; j++)
        {
            int r = rand() % (4000000000);
            if (r > 1000000000)
            {
                ioopm_linked_list_malloc_contains(malloc_list1, (elem_t){.int_v = r});
            }
            else if (r > 2000000000)
            {
                ioopm_linked_list_malloc_contains(malloc_list2, (elem_t){.int_v = r});
            }
            else if (r > 3000000000)
            {
                ioopm_linked_list_malloc_contains(malloc_list3, (elem_t){.int_v = r});
            }
            else
            {
                ioopm_linked_list_malloc_contains(malloc_list4, (elem_t){.int_v = r});
            }
        }

        ioopm_linked_list_malloc_destroy(malloc_list1);
        ioopm_linked_list_malloc_destroy(malloc_list2);
        ioopm_linked_list_malloc_destroy(malloc_list3);
        ioopm_linked_list_malloc_destroy(malloc_list4);

        clock_gettime(CLOCK_MONOTONIC, &tend);

        fprintf(f, "%.5f\n", ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec));
    }
    fclose(f);
    return 0;
}
