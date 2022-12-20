CC=gcc 
FLAGS=-Wall -I src -g
OBJECTS= hash_table.o linked_list.o cart.o inventory.o ask.o general_TUI.o
%.o: %.c %.h data_structure.h iterator.h hash_table_internal.h linked_list_internal.h
	$(CC) $(FLAGS) $< -c

store: $(OBJECTS) store.c
	$(CC) $(FLAGS) $^ -o $@

memcheck_store: store
	valgrind --leak-check=full --show-leak-kinds=all ./store

stock: store
	rm -f inventory.bin
	touch inventory.bin
	gcc make_stock.c -o make_stock
	valgrind --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./make_stock
	
inv_clear:
	rm -f inventory.bin
	touch inventory.bin

clean:
	rm -f *.o
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -f *.out
	rm -f make_stock store

.PHONY: clean stock inv_clear
