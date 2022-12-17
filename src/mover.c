#include "structs.h"
#include "metadata.h"
#include "mover.h"
#include <string.h>
#include <stdio.h>

void move(heap_t *ext_heap)
{
    page_t **passive_pages = get_passive_pages(ext_heap->internal_heap);
}

page_t *get_moveto_page(unsigned int bytes, page_t **new_pages, int len)
{
    for (size_t i = 0; i < len; i++)
    {
        make_active(new_pages[i]);
        if (has_room(new_pages[i], bytes))
        {
            return new_pages[i];
        }
    }
    return NULL;
}

void move_data_block(unsigned int bytes, metadata_t *md, page_t *new_page, void *new_data_ptr, void **data_ptr)
{
    assert(new_data_ptr > new_page->memory_block && new_data_ptr < new_page->memory_block + new_page->offset);
    *md = set_forward_address(new_data_ptr);
    memcpy(new_data_ptr, *data_ptr, bytes);
    *data_ptr = new_data_ptr;
}

void do_move(void **data_ptr, page_t **new_pages, int len)
{
    metadata_t *md = ((metadata_t *)*data_ptr) - 1;
    if (is_format_vector(*md))
    {
        size_t bytes = get_size_struct(*md);
        int len_fv = 0;
        bool *format_vector = get_format_vector(*md, &len_fv);

        page_t *new_page = get_moveto_page(bytes, new_pages, len);
        assert(new_page);

        void *new_data_ptr = page_alloc_struct(new_page, format_vector, len_fv, bytes);

        move_data_block(bytes, md, new_page, new_data_ptr, data_ptr);

        for (size_t i = 0; i < len_fv; i++)
        {
            if (format_vector[i])
            {
                void **internal_ptr = (void **)(*data_ptr + 8 * i);
                metadata_t *internal_md = ((metadata_t *)*internal_ptr) - 1;

                assert(*internal_ptr);

                if (is_forward_address(*internal_md))
                {
                    *internal_ptr = get_forward_address(*internal_md);
                }
                else
                {
                    do_move(internal_ptr, new_pages, len);
                }
            }
        }
        free(format_vector);
    }
    else if (is_data_size(*md))
    {
        size_t bytes = get_data_size(*md);

        page_t *new_page = get_moveto_page(bytes, new_pages, len);
        assert(new_page);

        void *new_data_ptr = page_alloc_data(new_page, bytes);

        move_data_block(bytes, md, new_page, new_data_ptr, data_ptr);
    }
    else
    {
        assert(false);
    }
}