#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include "gc.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    FILE *f = fopen("perf_comparison/times_GC_vs_free.csv", "w");
    struct timespec tstart = {0, 0}, tend = {0, 0};

    for (size_t i = 50000; i < 100000; i+=1000)
    {
        heap_t *h = h_init(i*16*2 + 10, false, 1.0);

        void *start_gc_2 = h_alloc_struct(h, "*");
        void *cursor_gc_2 = start_gc_2;

        void *start_free = calloc(1, 8);
        void *cursor_free = start_free;

        for (size_t j = 0; j < i; j++)
        {
            *(void **)cursor_gc_2 = h_alloc_struct(h, "*");
            cursor_gc_2 = *(void **)cursor_gc_2;

            *(void **)cursor_free = calloc(1, 8);
            cursor_free = *(void **)cursor_free;
        }

        start_gc_2 = NULL;
        cursor_gc_2 = NULL;
        *(void **)cursor_free = NULL;

        clock_gettime(CLOCK_MONOTONIC, &tstart);

        h_gc(h);

        clock_gettime(CLOCK_MONOTONIC, &tend);

        fprintf(f, "%lu, %.5f, ", i, ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec));

        h_delete(h);


        clock_gettime(CLOCK_MONOTONIC, &tstart);

        void **next = start_free;

        while (*next != NULL)
        {
            void *tmp = next;
            next = (void **) *next;
            free(tmp);
        }
        free(next);

        clock_gettime(CLOCK_MONOTONIC, &tend);

        fprintf(f, "%.5f\n", ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec));
    }

    fclose(f);

    return 0;
}