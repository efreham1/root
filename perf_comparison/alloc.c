#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include "gc.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    FILE *f = fopen("perf_comparison/times.csv", "w");
    struct timespec tstart = {0, 0}, tend = {0, 0};

    for (size_t i = 1; i < 101; i++)
    {
        heap_t *h = h_init(i * 50 * 1024 + 10, true, 1.0);

        clock_gettime(CLOCK_MONOTONIC, &tstart);

        for (size_t j = 0; j < i * 50; j++)
        {
            h_alloc_data(h, 2040);
        }

        clock_gettime(CLOCK_MONOTONIC, &tend);

        fprintf(f, "%lu, %.5f, ", i * 50, ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec));
        h_delete(h);

        void *ptrs1[i * 50];
        clock_gettime(CLOCK_MONOTONIC, &tstart);

        for (size_t j = 0; j < i * 50; j++)
        {
            ptrs1[j] = malloc(2048);
        }

        clock_gettime(CLOCK_MONOTONIC, &tend);

        fprintf(f, "%.5f\n", ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec));

        for (size_t j = 0; j < i * 50; j++)
        {
            free(ptrs1[j]);
        }
    }

    fclose(f);

    return 0;
}