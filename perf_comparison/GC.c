#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include "gc.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    FILE *f = fopen("perf_comparison/times_GC.csv", "w");
    struct timespec tstart = {0, 0}, tend = {0, 0};

    for (size_t i = 0; i < 101; i++)
    {
        heap_t *h = h_init(100 * 100 * 32 + 10, true, 1.0);
        void *ptrs[i * 100];

        for (size_t j = 0; j < i * 100 + 1; j++)
        {
            ptrs[j] = h_alloc_data(h, 24);
        }

        for (size_t j = i * 100; j < 100 * 100 + 1; j++)
        {
            h_alloc_data(h, 24);
        }

        clock_gettime(CLOCK_MONOTONIC, &tstart);

        h_gc(h);

        clock_gettime(CLOCK_MONOTONIC, &tend);

        fprintf(f, "%lu, %.5f\n", i, ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec));

        h_delete(h);
    }

    fclose(f);

    return 0;
}