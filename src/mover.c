#include "structs.h"
#include "metadata.h"
#include "mover.h"
#include <string.h>
#include <stdio.h>

void reset_all_been_visited(void *data_ptr, internal_heap_t *i_heap) //This function resets all visited (still active) blocks of memory to be unvisited
{
    metadata_t *md = ((metadata_t *)data_ptr) - 1; //get metadata for current memoryblock
    if (is_format_vector(*md))
    {
        *md = reset_been_visited(*md); //do the reset
        assert(!is_been_visited(*md));
        int len_fv = 0;
        bool *format_vector = get_format_vector(*md, &len_fv); //get the block's format vector

        for (size_t i = 0; i < len_fv; i++)
        {
            if (format_vector[i]) //if it's a pointer
            {
                void *internal_ptr = (void *)*((void **)data_ptr + i); //cast to the internal ptr
                if (is_valid_ptr(i_heap, internal_ptr))
                {
                    reset_all_been_visited(internal_ptr, i_heap); // if it's valid recursivly call the function
                }
            }
        }

        free(format_vector);
    }
    else if (is_data_size(*md))
    {
        *md = reset_been_visited(*md); // nothing more is needed since there are no additional ptrs
    }
}

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
        for (size_t i = 0; i < act_len; i++) //iterate over the active pages
        {
            for (size_t j = 0; j < ptrs_len && active_pages[i] != NULL; j++)
            //iterate over the stack_ptrs until a ptr to the page is found or until there are no more stack ptrs
            {
                if (is_ptr_to_page(active_pages[i], *stack_ptrs[j]) && *stack_ptrs[j] == proof_reading_arr[j])
                //if the ptr is a ptr to the page and the ptr matches its proofreading ptr
                {
                    static_pages[len++] = active_pages[i];
                    active_pages[i] = NULL; // the page is no longer active since it's static
                }
            }
        }
    }

    for (size_t i = 0; i < ptrs_len; i++)
    {
        if (*stack_ptrs[i] == proof_reading_arr[i])
        //if a ptr matches its proofreading ptr call do_move on it
        {
            do_move(stack_ptrs[i], passive_pages, pass_len, static_pages, len, i_heap);
        }
    }

    for (size_t i = 0; i < act_len; i++)
    //make all active pages passive
    {
        if (active_pages[i] != NULL)
        {
            make_passive(active_pages[i]);
            i_heap->num_active_pages--;
        }
    }

    for (size_t i = 0; i < ptrs_len; i++)
    //reset all the ptrs
    {
        if (*stack_ptrs[i] == proof_reading_arr[i])
        {
            reset_all_been_visited(*stack_ptrs[i], i_heap);
        }
    }

    free(passive_pages);
    free(active_pages);
}

page_t *get_moveto_page(unsigned int bytes, page_t **new_pages, int len, internal_heap_t *i_heap, int static_len)
{
    for (size_t i = 0; i < len; i++)
    //finds the first page to have room
    {
        if (has_room(new_pages[i], bytes))
        {
            return new_pages[i];
        }
    }
    for (size_t i = 0; i < len; i++)
    //if no active page has room, make first passive page active
    {
        if (!is_active(new_pages[i]))
        {
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
    assert(is_ptr_to_page(new_page, new_data_ptr));
    *md = set_forward_address(new_data_ptr);
    memcpy(new_data_ptr, *data_ptr, bytes);
    //copy over the actual data
    *data_ptr = new_data_ptr;
    //update the ptr
}

bool is_movable(void *ptr, page_t **static_pages, int static_len)
{
    for (size_t i = 0; i < static_len; i++) //iterate over static pages
    {
        if (is_ptr_to_page(static_pages[i], ptr))
        //if the ptr points to a static page it can't be moved
        {
            return false;
        }
    }
    //the ptr didn't point to a static page and is therefore movable
    return true;
}

void do_move(void **data_ptr, page_t **new_pages, int len, page_t **static_pages, int static_len, internal_heap_t *i_heap)
{
    metadata_t *md = ((metadata_t *)*data_ptr) - 1; //get metadata
    if (is_been_visited(*md))
    {
        //the mem block has already been visited this mover-run so nothing needs to be done
        return;
    }
    else if (is_format_vector(*md)) //"if is struct"
    {
        int len_fv = 0;
        bool *format_vector = get_format_vector(*md, &len_fv);

        if (is_movable(*data_ptr, static_pages, static_len))
        {
            size_t bytes = get_size_struct(*md); //get the size in bytes of the struct

            page_t *new_page = get_moveto_page(bytes, new_pages, len, i_heap, static_len);
            assert(new_page);

            void *new_data_ptr = page_alloc_struct(new_page, format_vector, len_fv, bytes);
            //allocate an identical block on the new page

            move_data_block(bytes, md, new_page, new_data_ptr, data_ptr);
            //move the old block
        }
        else
        {
            *md = set_been_visited(*md);
        }

        for (size_t i = 0; i < len_fv; i++)
        //iterate over the format vector
        {
            if (format_vector[i]) // if the data is a ptr
            {
                void **internal_ptr = (void **)(*data_ptr + 8 * i); //get the ptr
                if (is_valid_ptr(i_heap, *internal_ptr)) // check its validity
                {
                    do_move(internal_ptr, new_pages, len, static_pages, static_len, i_heap);
                    //recursivly call the function
                }
            }
        }

        free(format_vector);
    }
    else if (is_data_size(*md))
    {
        if (is_movable(*data_ptr, static_pages, static_len))
        //same as before but allocating data instead of a struct
        {
            size_t bytes = get_data_size(*md);

            page_t *new_page = get_moveto_page(bytes, new_pages, len, i_heap, static_len);
            assert(new_page);

            void *new_data_ptr = page_alloc_data(new_page, bytes);

            move_data_block(bytes, md, new_page, new_data_ptr, data_ptr);
        }
        else
        {
            *md = set_been_visited(*md);
        }
    }
    else if (is_forward_address(*md))
    {
        //the block has already been moved so this ptr just neds to be relinked.
        *data_ptr = get_forward_address(*md);
    }
    else
    {
        //something went wrong
        assert(false);
    }
}