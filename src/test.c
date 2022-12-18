#include <stdio.h>
#include "stack_scanner.h"

int main(int argc, char const *argv[])
{
    int *i = calloc(1, sizeof(int));
    int *j = calloc(1, sizeof(int));
    int *k = calloc(1, sizeof(int));
    int *m = calloc(1, sizeof(int));

    int len = 0;
    void ***stack_ptrs = stack_addresses((void *) m, (void *) i, &len);

    free(i);
    free(j);
    free(k);
    free(m);
    free(stack_ptrs);
    return 0;
}