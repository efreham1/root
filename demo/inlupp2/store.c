#include <stdbool.h>
#include <ctype.h>
#include "ask.h"
#include "cart.h"
#include "inventory.h"
#include "iterator.h"
#include "gc.h"

void TUI_inventory_list_merch(ioopm_inventory_t *inventory, heap_t *h);
void TUI_cart_list_contents(ioopm_cart_t *cart, heap_t *h);
void TUI_inventory_list_merch_theoretical_stock(ioopm_inventory_t *inventory, heap_t *h);

bool cart_eq_fun(elem_t a, elem_t b)
{
    return a.ptr_v == b.ptr_v;
}

void TUI_cart_add(ioopm_list_t *cart_list, ioopm_inventory_t *inventory, heap_t *h)
{
    ioopm_cart_t *cart = ask_cart_number(cart_list, h);
    char *merch_name;
    int cost;
    int pieces;
    TUI_inventory_list_merch_theoretical_stock(inventory, h);
    ioopm_ask_cart_merch(inventory, &merch_name, &cost, &pieces, h);
    ioopm_cart_add(cart, merch_name, cost, pieces, h);
    inventory_merch_t *merch = ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = merch_name})->ptr_v;
    merch->theoretical_stock -= pieces;
}

void TUI_cart_remove(ioopm_list_t *cart_list, heap_t *h)
{
    ioopm_cart_t *cart = ask_cart_number(cart_list, h);
    TUI_cart_list_contents(cart, h);
    char *merch_name = ioopm_ask_existing_cart_merch_name(cart, h);
    ioopm_cart_remove(cart, merch_name);
}

void TUI_cart_get_cost(ioopm_list_t *cart_list, heap_t *h)
{
    ioopm_cart_t *cart = ask_cart_number(cart_list, h);
    int cost = ioopm_cart_get_cost(cart);
    printf("The cost of all the contents in your cart is %d.%d SEK\n", cost / 100, cost % 100);
}

void TUI_cart_list_list_contents(ioopm_list_t *cart_list, heap_t *h)
{
    ioopm_cart_t *cart = ask_cart_number(cart_list, h);
    TUI_cart_list_contents(cart, h);
}

void TUI_cart_list_contents(ioopm_cart_t *cart, heap_t *h)
{
    ioopm_list_t *all_merch = ioopm_cart_get_merch(cart);
    if (ioopm_linked_list_is_empty(all_merch))
    {
        printf("\n___Cart is empty___\n\n");
        return;
    }

    ioopm_list_iterator_t *merch_iterator = ioopm_list_iterator(all_merch, h);
    for (int i = 0; i < ioopm_linked_list_size(all_merch); i++)
    {
        cart_merch_t *current_merch = ioopm_iterator_current(merch_iterator).ptr_v;
        char *name = current_merch->name;
        int pcs = current_merch->pcs;
        printf("%d) %s: %d pcs\n", i + 1, name, pcs);
        ioopm_iterator_next(merch_iterator);
    }
}

void TUI_inventory_add_merch(ioopm_inventory_t *inventory, heap_t *h)
{

    char *name;
    char *desc;
    int price;

    ioopm_ask_new_inventory_merch(inventory->warehouse, &name, &desc, &price, h);

    ioopm_inventory_add_merchandise(inventory, name, desc, price, h);

    if (ioopm_hash_table_has_key(inventory->warehouse, (elem_t){.ptr_v = name}))
    {
        printf("%s succesfully added to warehouse\n", name);
    }
}

void TUI_inventory_list_merch_theoretical_stock(ioopm_inventory_t *inventory, heap_t *h)
{
    ioopm_list_t *merch_list = ioopm_inventory_get_merchandise_list(inventory, h);
    if (ioopm_linked_list_is_empty(merch_list))
    {
        printf("\n__________The inventory is empty!__________\n\n");
        return;
    }

    ioopm_list_iterator_t *iterator = ioopm_list_iterator(merch_list, h);

    int i = 1;
    while (true)
    {
        char *current_name = ioopm_iterator_current(iterator).ptr_v;
        inventory_merch_t *current_merch = ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = current_name})->ptr_v;
        int current_price = current_merch->price;
        printf("%d) %s %d.%d SEK  %d in stock\n", i, current_name, current_price / 100, current_price % 100, current_merch->theoretical_stock);
        if (!ioopm_iterator_has_next(iterator))
            break;
        ioopm_iterator_next(iterator);
        if (i % 20 == 0)
        {
            char *answer = ioopm_ask_question_string("\nDo you wish to keep printing merchandise? [y/n]\n", h);
            if (toupper(*answer) != 'Y')
            {

                break;
            }
        }
        i++;
    }
}

void TUI_inventory_list_merch(ioopm_inventory_t *inventory, heap_t *h)
{

    ioopm_list_t *merch_list = ioopm_inventory_get_merchandise_list(inventory, h);
    if (ioopm_linked_list_is_empty(merch_list))
    {
        printf("\n__________The inventory is empty!__________\n\n");
        return;
    }

    ioopm_list_iterator_t *iterator = ioopm_list_iterator(merch_list, h);

    int i = 1;
    while (true)
    {

        char *current_name = ioopm_iterator_current(iterator).ptr_v;
        printf("\n%d\n", current_name == NULL);
        inventory_merch_t *current_merch = ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = current_name})->ptr_v;
        int current_price = current_merch->price;
        printf("%d) %s %d.%d SEK  %d in stock\n", i, current_name, current_price / 100, current_price % 100, current_merch->total_stock);
        if (!ioopm_iterator_has_next(iterator))
            break;
        ioopm_iterator_next(iterator);
        if (i % 20 == 0)
        {
            char *answer = ioopm_ask_question_string("\nDo you wish to keep printing merchandise? [y/n]\n", h);
            if (toupper(*answer) != 'Y')
            {

                break;
            }
        }
        i++;
    }
}

void TUI_inventory_remove_merch(ioopm_inventory_t *inventory, heap_t *h)
{
    TUI_inventory_list_merch(inventory, h);

    char *merch_name = ioopm_ask_existing_inventory_merch(inventory->warehouse, h);
    ioopm_inventory_remove_merchandise(inventory, merch_name);
}

void TUI_inventory_edit_merch(ioopm_inventory_t *inventory, heap_t *h)
{
    TUI_inventory_list_merch(inventory, h);
    if (ioopm_hash_table_is_empty(inventory->warehouse))
    {
        return;
    }

    char *old_name = ioopm_ask_existing_inventory_merch(inventory->warehouse, h);

    char *new_name = NULL;
    char *new_desc = NULL;
    int new_price = 0;
    char action = '\0';

    while (action != 'F')
    {
        char *choice =
            "\nChoose what to edit:\n\n"
            "Edit [n]ame\n"
            "Edit [d]escription\n"
            "Edit [p]rice\n"
            "[F]inished editing";
        char *str = ioopm_ask_question_string(choice, h);
        action = toupper(*str);
        switch (action)
        {
        case 'N':
            if (new_name != NULL)
            {
            }

            new_name = ioopm_ask_question_string("\nEnter new name:\n", h);
            break;

        case 'D':
            if (new_desc != NULL)
            {
            }
            new_desc = ioopm_ask_question_string("\nEnter new description:\n", h);
            break;

        case 'P':
            new_price = ioopm_ask_question_int("\nEnter new price:\n", h);
            break;

        case 'F':
            printf("\nMerchandise has been successfully edited.\n");
            break;

        default:
            printf("\nInvalid input\n");
            break;
        }
    }
    ioopm_inventory_edit_merchandise(inventory, old_name, new_name, new_desc, new_price, h);
}

static void show_detailed_stock(ioopm_inventory_t *inventory, char *merch_name, heap_t *h)
{
    ioopm_list_t *storage_locations = ioopm_inventory_storage_locations_merch(inventory, merch_name);

    if (!ioopm_linked_list_is_empty(storage_locations))
    {

        ioopm_list_iterator_t *iterator = ioopm_list_iterator(storage_locations, h);

        printf("\nTotal stock of %s: %d\n\nStock per shelf:\n", merch_name, ioopm_inventory_get_stock(inventory, merch_name));

        while (true)
        {
            storage_location_t storage_location = *(storage_location_t *)ioopm_iterator_current(iterator).ptr_v;
            printf("%s) %d\n", storage_location.shelf, storage_location.stock);
            if (!ioopm_iterator_has_next(iterator))
                break;
            ioopm_iterator_next(iterator);
        }
    }
    else
    {
        printf("\n%s is out of stock.\n", merch_name);
    }
}

void TUI_inventory_show_stock(ioopm_inventory_t *inventory, heap_t *h)
{
    TUI_inventory_list_merch(inventory, h);
    if (ioopm_hash_table_is_empty(inventory->warehouse))
        return;
    char *merch_name = ioopm_ask_existing_inventory_merch(inventory->warehouse, h);

    show_detailed_stock(inventory, merch_name, h);
}

void TUI_inventory_replenish_stock(ioopm_inventory_t *inventory, heap_t *h)
{
    TUI_inventory_list_merch(inventory, h);
    if (ioopm_hash_table_is_empty(inventory->warehouse))
        return;
    char *merch_name = ioopm_ask_existing_inventory_merch(inventory->warehouse, h);

    char *new_or_old = ioopm_ask_question_string("Would you like to add merchandise to a new shelf?", h);

    if (toupper(*new_or_old) != 'N')
    {
        char *shelf = ioopm_ask_new_shelf(inventory->used_shelves, h);
        int amount = ioopm_ask_question_u_int("How many of the merchandise would you like to add?", h);
        ioopm_inventory_replenish_new_shelf_stock(inventory, merch_name, amount, shelf, h);
    }
    else
    {
        char *shelf = ioopm_ask_old_shelf(inventory->used_shelves, h);
        int amount = ioopm_ask_question_u_int("How many of the merchandise would you like to add?", h);
        ioopm_inventory_replenish_existing_shelf_stock(inventory, merch_name, amount, shelf, h);
    }
}

void TUI_inventory_unstock(ioopm_inventory_t *inventory, heap_t *h)
{
    char *merch_name = ioopm_ask_existing_inventory_merch(inventory->warehouse, h);
    show_detailed_stock(inventory, merch_name, h);
    printf("Which shelf would you like to remove %s from?\n", merch_name);
    char *shelf = ioopm_ask_old_shelf(inventory->used_shelves, h);
    int amount = ioopm_ask_No_stock(inventory, merch_name, shelf, h);
    ioopm_inventory_unstock(inventory, merch_name, amount, shelf, h);
}

void do_checkout(ioopm_inventory_t *inventory, ioopm_list_t *cart_list, heap_t *h)
{
    ioopm_cart_t *cart = ask_cart_number(cart_list, h);

    ioopm_list_t *cart_merch = ioopm_cart_get_merch(cart);
    if (!ioopm_linked_list_is_empty(cart_merch))
    {
        ioopm_list_iterator_t *iterator = ioopm_list_iterator(cart_merch, h);

        while (true)
        {
            cart_merch_t *merch = ioopm_iterator_current(iterator).ptr_v;

            if (!ioopm_hash_table_has_key(inventory->warehouse, (elem_t){.ptr_v = merch->name}))
            {
                printf("\nSomething went wrong, %s wasn't checked out\n", merch->name);
                ioopm_cart_remove(cart, merch->name);
            }
            else
            {
                ioopm_inventory_unstock(inventory, merch->name, merch->pcs, NULL, h);
                printf("\nYou bought %d pcs of %s. \nPrice: %d.%d SEK\n", merch->pcs, merch->name, (merch->pcs * merch->price) / 100, (merch->pcs * merch->price) % 100);
            }

            if (!ioopm_iterator_has_next(iterator))
            {
                break;
            }
            else
            {
                ioopm_iterator_next(iterator);
            }
        }
        printf("\nTotal price: %d.%d SEK\n", cart->cost / 100, cart->cost % 100);
        ioopm_cart_clear(cart);
    }
    int idx = ioopm_linked_list_get_index(cart_list, (elem_t){.ptr_v = cart});
    ioopm_linked_list_remove(cart_list, idx);
    ioopm_linked_list_insert(cart_list, idx, (elem_t){.ptr_v = NULL}, h);
    return;
}

void add_cart(ioopm_list_t *cart_list, heap_t *h)
{
    ioopm_cart_t *cart = ioopm_cart_create(h);
    ioopm_linked_list_append(cart_list, (elem_t){.ptr_v = cart}, h);
}

void clear_and_return_cart(ioopm_cart_t *cart, ioopm_inventory_t *inventory, heap_t *h)
{
    ioopm_list_iterator_t *iter = ioopm_list_iterator(cart->contents, h);

    for (int i = 0; i < ioopm_linked_list_size(cart->contents); i++)
    {
        cart_merch_t *cart_merch = ioopm_iterator_current(iter).ptr_v;
        if (ioopm_hash_table_has_key(inventory->warehouse, (elem_t){.ptr_v = cart_merch->name}))
        {
            inventory_merch_t *inventory_merch = ioopm_hash_table_lookup(inventory->warehouse, (elem_t){.ptr_v = cart_merch->name})->ptr_v;
            inventory_merch->theoretical_stock += cart_merch->pcs;
        }
        ioopm_iterator_next(iter);
    }

    ioopm_cart_clear(cart);
}

void TUI_cart_clear(ioopm_list_t *cart_list, ioopm_inventory_t *inventory, heap_t *h)
{
    ioopm_cart_t *cart = ask_cart_number(cart_list, h);
    clear_and_return_cart(cart, inventory, h);
}

bool int_eq_fun(elem_t a, elem_t b)
{
    return a.int_v == b.int_v;
}

int event_loop(ioopm_inventory_t *inventory, ioopm_list_t *cart_list, heap_t *h)
{
    char *admin_menu =
        "-----------------Admin menu-----------------\n"
        "Please chose an option by typing its number:\n"
        "1. Add a merchandise to the inventory.\n"
        "2. List all merchandise in the inventory.\n"
        "3. Remove merchandise from the inventory.\n"
        "4. Edit merchandise in the inventory.\n"
        "5. Replenish merchandise in the inventory.\n"
        "6. Remove merchandise from stock.\n"
        "7. Add merchandise to your cart.\n"
        "8. Remove merchandise from your cart.\n"
        "9. Get the cost of your cart's contents.\n"
        "10. List the cart's contents.\n"
        "11. CLear the cart's contents.\n"
        "12. Checkout your cart.\n"
        "13. Exit the menu.\n"
        "14. Show detailed stock of merchandise.\n"
        "15. Create a new cart.\n";

    ioopm_list_t *admin_options = ioopm_linked_list_create(int_eq_fun, h);
    for (int i = 1; i < 16; i++)
    {
        ioopm_linked_list_append(admin_options, (elem_t){.int_v = i}, h);
    }

    bool admin_access = ioopm_ask_admin_access(inventory->password, h);
    while (admin_access)
    {
        switch (ioopm_ask_menu(admin_menu, admin_options, h))
        {
        case 1:
            TUI_inventory_add_merch(inventory, h);
            break;

        case 2:
            TUI_inventory_list_merch(inventory, h);
            break;

        case 3:
            TUI_inventory_remove_merch(inventory, h);
            break;

        case 4:
            TUI_inventory_edit_merch(inventory, h);
            break;

        case 5:
            TUI_inventory_replenish_stock(inventory, h);
            break;

        case 6:
            TUI_inventory_unstock(inventory, h);
            break;

        case 7:
            TUI_cart_add(cart_list, inventory, h);
            break;

        case 8:
            TUI_cart_remove(cart_list, h);
            break;

        case 9:
            TUI_cart_get_cost(cart_list, h);
            break;

        case 10:
            TUI_cart_list_list_contents(cart_list, h);
            break;

        case 11:
            TUI_cart_clear(cart_list, inventory, h);
            break;

        case 12:
            do_checkout(inventory, cart_list, h);
            break;

        case 13:
            admin_access = false;
            break;

        case 14:
            TUI_inventory_show_stock(inventory, h);
            break;
        case 15:
            add_cart(cart_list, h);
            break;
        }
    }

    return 0;
}

void destroy_carts(ioopm_list_t *cart_list, ioopm_inventory_t *inventory, heap_t *h)
{
    ioopm_list_iterator_t *iter = ioopm_list_iterator(cart_list, h);
    for (int i = 0; i < ioopm_linked_list_size(cart_list); i++)
    {
        void *curr_ptr = ioopm_iterator_current(iter).ptr_v;
        if (curr_ptr != NULL)
        {
            clear_and_return_cart((ioopm_cart_t *)curr_ptr, inventory, h);
        }
        ioopm_iterator_next(iter);
    }
}

int main(void)
{
    heap_t *heap = h_init(32768, true, 0.5);
    ioopm_inventory_t *inventory = ioopm_inventory_load("demo/inlupp2/inventory.bin", heap);
    ioopm_list_t *cart_list = ioopm_linked_list_create(cart_eq_fun, heap);

    if (event_loop(inventory, cart_list, heap) == 1)
        return 1;

    destroy_carts(cart_list, inventory, heap);
    ioopm_inventory_save(inventory, "demo/inlupp2/inventory.bin");
    h_delete(heap);
    return 0;
}
