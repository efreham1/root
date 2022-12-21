#include "structs.h"
#include "metadata.h"
#include "mover.h"
#include <string.h>
#include <stdio.h>

void move(internal_heap_t *i_heap, void ***stack_ptrs, int ptrs_len, bool unsafe_stack, void **proof_reading_arr)
{
    int pass_len = 0;
    page_t **passive_pages = get_passive_pages(i_heap, &pass_len);

    int act_len = 0;
    page_t **active_pages = get_active_pages(i_heap, &act_len);

    page_t *static_pages[act_len];
    int len = 0;

    if (unsafe_stack)
    {
        for (size_t i = 0; i < act_len; i++)
        {
            for (size_t j = 0; j < ptrs_len && active_pages[i] != NULL; j++)
            {
                if (is_ptr_to_page(active_pages[i], *stack_ptrs[j]) && *stack_ptrs[j] == proof_reading_arr[j])
                {
                    static_pages[len++] = active_pages[i];
                    active_pages[i] = NULL;
                }
            }
        }
    }

    for (size_t i = 0; i < ptrs_len; i++)
    {
        if (*stack_ptrs[i] == proof_reading_arr[i])
        {
            do_move(stack_ptrs[i], passive_pages, pass_len, static_pages, len, i_heap);
        }
    }

    for (size_t i = 0; i < act_len; i++)
    {
        if (active_pages[i] != NULL)
        {
            make_passive(active_pages[i]);
            i_heap->num_active_pages--;
            printf("\nmade page passive\n");
        }
    }

    free(passive_pages);
    free(active_pages);
}

page_t *get_moveto_page(unsigned int bytes, page_t **new_pages, int len, internal_heap_t *i_heap, int static_len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (has_room(new_pages[i], bytes))
        {
            return new_pages[i];
        }
        else if (!is_active(new_pages[i]))
        {
            printf("\nmade page active\n"); 
            make_active(new_pages[i]);
            i_heap->num_active_pages++;
            assert(i_heap->num_active_pages <= i_heap->num_pages - static_len);
            return new_pages[i];
        }
    }
    return NULL;
}

void move_data_block(unsigned int bytes, metadata_t *md, page_t *new_page, void *new_data_ptr, void **data_ptr)
{
    assert(new_data_ptr > new_page->memory_block && new_data_ptr < new_page->memory_block + new_page->offset && is_active(new_page));
    *md = set_forward_address(new_data_ptr);
    memcpy(new_data_ptr, *data_ptr, bytes);
    *data_ptr = new_data_ptr;
}

bool is_movable(void *ptr, page_t **static_pages, int static_len)
{
    bool flag = true;
    for (size_t i = 0; i < static_len; i++)
    {
        if (is_ptr_to_page(static_pages[i], ptr))
        {
            flag = false;
        }
    }
    return flag;
}

void do_move(void **data_ptr, page_t **new_pages, int len, page_t **static_pages, int static_len, internal_heap_t *i_heap)
{
    metadata_t *md = ((metadata_t *)*data_ptr) - 1;
    if (is_format_vector(*md))
    {
        metadata_t og_md = *md;
        bool reset_md = false;

        int len_fv = 0;
        bool *format_vector = get_format_vector(*md, &len_fv);

        if (is_movable(*data_ptr, static_pages, static_len))
        {
            size_t bytes = get_size_struct(*md);

            page_t *new_page = get_moveto_page(bytes, new_pages, len, i_heap, static_len);
            assert(new_page);

            void *new_data_ptr = page_alloc_struct(new_page, format_vector, len_fv, bytes);

            move_data_block(bytes, md, new_page, new_data_ptr, data_ptr);
        }
        else
        {
            *md = set_been_visited();
            reset_md = true;
        }

        for (size_t i = 0; i < len_fv; i++)
        {
            if (format_vector[i])
            {
                void **internal_ptr = (void **)(*data_ptr + 8 * i);
                if (*internal_ptr != NULL && is_valid_ptr(i_heap, *internal_ptr))
                {
                    do_move(internal_ptr, new_pages, len, static_pages, static_len, i_heap);
                }
            }
        }

        if (reset_md)
        {
            *md = og_md;
        }

        free(format_vector);
    }
    else if (is_data_size(*md))
    {
        if (is_movable(*data_ptr, static_pages, static_len))
        {
            size_t bytes = get_data_size(*md);

            page_t *new_page = get_moveto_page(bytes, new_pages, len, i_heap, static_len);
            assert(new_page);

            void *new_data_ptr = page_alloc_data(new_page, bytes);

            move_data_block(bytes, md, new_page, new_data_ptr, data_ptr);
        }
    }
    else if (is_forward_address(*md))
    {
        *data_ptr = get_forward_address(*md);
    }
    else if (is_been_visited(*md))
    {
        return;
    }
    else
    {
        assert(false);
    }
}