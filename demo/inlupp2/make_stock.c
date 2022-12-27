#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(void)
{
    FILE *f = fopen("demo/inlupp2/Stock.txt", "w");
    assert(f != NULL);

    fprintf(f, "y\n1234\n");
    for (int i = 1; i < 5; i++)
    {
        fprintf(f, "1\nvara%d\nbeskrivning\n9999\n5\n", i);
        if (i > 20)
        {
            int freq = (i-1)/20;
            for (int i = 0; i < freq; i++)
            {
                fprintf(f, "Y\n");
            }
        }
        fprintf(f, "vara%d\ny\nT%d\n56\n", i, i);
        
    }
    fprintf(f, "13\n");
    fclose(f);
    return 0;
}