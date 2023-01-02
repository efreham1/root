SDIR = src
EDIR = exe
ODIR = obj
TDIR = test
DDIR = demo

CC = gcc

FLAGS = -I src -Wall -g -pg -fprofile-arcs -ftest-coverage

MODULES = gc page heap metadata mover stack_scanner hash_table linked_list

MODULESTEST = gc page heap metadata mover stack_scanner

OBJS = $(patsubst %,$(ODIR)/%.o,$(MODULES))
TESTS = $(patsubst %,$(EDIR)/test_%,$(MODULESTEST))

INLUPP2 = cart.o inventory.o ask.o general_TUI.o

INLUPP2_OBJS = $(patsubst %,$(DDIR)/inlupp2/%,$(INLUPP2))

$(ODIR)/%.o: $(SDIR)/%.c $(SDIR)/%.h
	$(CC) $(FLAGS) $< -c -o $@ 

$(EDIR)/%: $(SDIR)/%.c $(OBJS)
	$(CC) $(FLAGS) $^ -o $@

$(EDIR)/test_%: $(TDIR)/test_%.c $(OBJS)
	$(CC) $(FLAGS) $^ -lcunit  -o $@

test_%: $(EDIR)/test_%
	valgrind --gen-suppressions=all --error-exitcode=1 --leak-check=full --suppressions=./test/Cond_jump.supp --track-origins=yes --show-leak-kinds=all ./$^

test_cov_%: test_%
	gcovr -b obj -e src/hash_table -e src/linked_list
	gcovr obj -e src/hash_table -e src/linked_list

test_all: $(TESTS)
	$(patsubst %,make test_% && ,$(MODULESTEST)) true

test_cov_all: $(TESTS)
	make clean
	make test_all
	gcovr -b obj -e src/hash_table -e src/linked_list
	gcovr obj -e src/hash_table -e src/linked_list
	gcovr obj -e src/hash_table -e src/linked_list -x cov/cov.xml


$(DDIR)/inlupp2/%.o: $(DDIR)/inlupp2/%.c $(DDIR)/inlupp2/%.h
	$(CC) $(FLAGS) $< -c -o $@

$(DDIR)/inlupp2/store: $(DDIR)/inlupp2/store.c $(OBJS) $(INLUPP2_OBJS)
	$(CC) $(FLAGS) $^ -o $@

demo: $(DDIR)/inlupp2/store
	valgrind --gen-suppressions=all --error-exitcode=1 --leak-check=full --suppressions=./test/Cond_jump.supp --track-origins=yes --show-leak-kinds=all ./$(DDIR)/inlupp2/store

inv_clear:
	make -C demo/inlupp2 inv_clear

stock: $(DDIR)/inlupp2/store
	rm -f $(DDIR)/inlupp2/inventory.bin
	touch $(DDIR)/inlupp2/inventory.bin
	gcc $(DDIR)/inlupp2/make_stock.c -o $(DDIR)/inlupp2/make_stock
	valgrind --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(DDIR)/inlupp2/make_stock
	valgrind --gen-suppressions=all --error-exitcode=1 --leak-check=full --suppressions=./test/Cond_jump.supp --track-origins=yes --show-leak-kinds=all ./$(DDIR)/inlupp2/store < demo/inlupp2/Stock.txt

clean:
	rm -rf exe/*
	rm -rf obj/*
	rm -rf cov/*
	rm -f gmon.out
	make -C demo/inlupp2 clean

.PHONY: clean test_% demo inv_clear

.PRECIOUS: $(ODIR)/%.o $(EDIR)/test_% $(EDIR)/%
