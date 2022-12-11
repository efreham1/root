#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "metadata.h"

bool is_little_end()
{
    int num = 1;
    return *((char *)&num) == 1;
}

void get_data_id(metadata_t mdata, bool *id)
{
    assert(is_little_end());
    char *b = (char *)&mdata;
    id[0] = (*b & 1) == 1 ? true : false;
    printf("id1 = %d\n", id[0]);
    id[1] = (*b >> 1 & 1) == 1 ? true : false;
    printf("id2 = %d\n", id[1]);
}

bool is_format_vector(metadata_t mdata)
{
    bool id[2];
    get_data_id(mdata, id);
    return id[0] == true && id[1] == true;
}

bool is_data_size(metadata_t mdata)
{
    bool id[2];
    get_data_id(mdata, id);
    return id[0] == true && id[1] == false;
}

bool is_forward_address(metadata_t mdata)
{
    bool id[2];
    get_data_id(mdata, id);
    return id[0] == false && id[1] == false;
}

metadata_t set_forward_address(void *ptr)
{
    return (metadata_t)ptr;
}

size_t get_data_size(metadata_t mdata)
{
    assert(is_data_size(mdata));
    char *b = (char *)&mdata;
    size_t size = b[1];
    return size;
}

metadata_t set_data_size(size_t size)
{
    assert(is_little_end());
    metadata_t mdata = 0;
    char *b = (char *)&mdata;
    b[1] = size;
    b[0] = *b & ~(1 << 1);
    b[0] = *b | 1;
    return mdata;
}

bool *get_format_vector(metadata_t mdata, int *len)
{
    assert(is_format_vector(mdata));
    bool buf[61] = {false};
    char *b = (char *)&mdata;
    bool flag = false;
    int idx = 0;
    for (int i = 0; i < sizeof(metadata_t); i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (i != 0 || j > 1)
            {
                if (flag)
                {
                    buf[idx++] = (b[i] >> j) & 1;
                }
                else
                {
                    flag = (b[i] >> j) & 1;
                }
            }
        }
    }
    bool *format_vector = calloc(idx, sizeof(bool));
    *len = idx;
    for (size_t i = 0; i < idx; i++)
    {
        format_vector[i] = buf[i];
    }
    
    return format_vector;
}

metadata_t set_format_vector(bool *format_vector, size_t len)
{
    assert(is_little_end());
    metadata_t mdata = 0;
    char *b = (char *)&mdata;
    for (int i = sizeof(metadata_t) - 1; i >= 0; i--)
    {
        for (int j = 7; j >= 0; j--)
        {
            if (len > 0)
            {
                if (format_vector[len - 1])
                {
                    b[i] = b[i] | 1<<j;
                }
                else
                {
                    b[i] = b[i] & ~1<<j;
                }
                len--;
            }
            else
            {
                b[i] = b[i] | 1<<j;
                b[0] = b[0] | 1;
                b[0] = b[0] | 1 << 1;
                return mdata;
            }
        }
    }
    return mdata;
}