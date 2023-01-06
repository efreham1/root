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
    id[0] = (*b & 1) == 1;
    id[1] = (*b >> 1 & 1) == 1;
}

bool is_format_vector(metadata_t mdata)
{
    bool id[2];
    get_data_id(mdata, id);
    return id[0] && id[1];
}

bool is_data_size(metadata_t mdata)
{
    bool id[2];
    get_data_id(mdata, id);
    return id[0] && !id[1];
}

bool is_forward_address(metadata_t mdata)
{
    bool id[2];
    get_data_id(mdata, id);
    return !id[0] && !id[1];
}

metadata_t set_forward_address(void *ptr)
{
    return (metadata_t)ptr;
}

void *get_forward_address(metadata_t mdata)
{
    return (void *)mdata;
}

unsigned int get_data_size(metadata_t mdata)
{
    assert(is_data_size(mdata));
    unsigned int size;
    char *mb = (char *)&mdata;
    char *sb = (char *)&size;
    for (unsigned int i = 0; i < sizeof(unsigned int); i++)
    {
        sb[i] = mb[i + 1];
    }

    return size;
}

metadata_t set_data_size(unsigned int size)
{
    assert(is_little_end());
    metadata_t mdata = NULL;
    char *mb = (char *)&mdata;
    char *sb = (char *)&size;
    for (unsigned int i = 0; i < sizeof(unsigned int); i++)
    {
        mb[i + 1] = sb[i];
    }
    mb[0] = mb[0] & ~(1 << 1);
    mb[0] = mb[0] | 1;
    return mdata;
}

bool get_format_vector_idx(metadata_t mdata, int idx)
{
    char *b = (char *)&mdata;
    assert(idx <= 55);
    return (b[7 - idx / 8] >> (7 - idx % 8)) & (char)1;
}

metadata_t set_format_vector(bool *format_vector, unsigned int len)
{
    assert(is_little_end());
    assert(len <= 55);
    metadata_t mdata = NULL;
    char *b = (char *)&mdata;
    int idx = 0;
    for (int i = sizeof(metadata_t) - 1; i >= 1; i--)
    {
        for (int j = 7; j >= 0; j--)
        {
            if (i == 1 && j == 0)
            {
            }
            else if (len > idx)
            {
                if (format_vector[idx++])
                {
                    b[i] = b[i] | 1 << j;
                }
                else
                {
                    b[i] = b[i] & ~(1 << j);
                }
            }
            else
            {
                char *sb = (char *)&len;
                b[0] = b[0] | 1;
                b[0] = b[0] | 1 << 1;
                for (size_t i = 0; i < 5; i++)
                {
                    if (sb[0] >> i & 1)
                    {
                        b[0] = b[0] | 1 << (i + 3);
                    }
                    else
                    {
                        b[0] = b[0] & ~(1 << (i + 3));
                    }
                }

                if (sb[0] >> 5 & 1)
                {
                    b[1] = b[1] | 1;
                }
                else
                {
                    b[1] = b[1] & ~1;
                }

                return mdata;
            }
        }
    }
    return mdata;
}

unsigned int get_size_format_vector(metadata_t md)
{
    char *b = (char *)&md;
    unsigned int len = 0;
    char *sb = (char *)&len;
    for (size_t i = 0; i < 5; i++)
    {
        if (b[0] >> (i + 3) & 1)
        {
            sb[0] = sb[0] | 1 << i;
        }
        else
        {
            sb[0] = sb[0] & ~(1 << i);
        }
    }

    if (b[1] & 1)
    {
        sb[0] = sb[0] & 1 << 5;
    }
    else
    {
        sb[0] = sb[0] & ~(1 << 5);
    }

    return len;
}

bool get_visitation_bit(metadata_t md)
{
    assert(is_little_end());
    char *b = (char *)&md;
    return (*b >> 2 & 1) == 1;
}

metadata_t set_visitation_bit(metadata_t md, bool visitation_bit)
{
    assert(is_little_end());
        char *mb = (char *)&md;

    if (visitation_bit)
    {
        mb[0] = mb[0] | 1 << 2;
        return md;
    }
    else
    {
        mb[0] = mb[0] & ~(1 << 2);
        return md;
    }
}