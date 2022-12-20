SDIR = src
EDIR = exe
ODIR = obj
TDIR = test
DDIR = demo

CC = gcc

FLAGS = -I src -Wall -g -fprofile-arcs -ftest-coverage

MODULES = gc page heap metadata mover stack_scanner

OBJS = $(patsubst %,$(ODIR)/%.o,$(MODULES))
TESTS = $(patsubst %,$(EDIR)/test_%,$(MODULES))

INLUPP2 = hash_table.o linked_list.o cart.o inventory.o ask.o general_TUI.o

INLUPP2_OBJS = $(patsubst %,$(DDIR)/inlupp2/%,$(INLUPP2))

$(ODIR)/%.o: $(SDIR)/%.c $(SDIR)/%.h $(SDIR)/structs.h
	$(CC) $(FLAGS) $< -c -o $@ 

$(EDIR)/%: $(SDIR)/%.c $(OBJS) 
	$(CC) $(FLAGS) $^ -o $@

$(EDIR)/test_%: $(TDIR)/test_%.c $(OBJS)
	$(CC) $(FLAGS) $^ -lcunit  -o $@

test_%: $(EDIR)/test_%
	valgrind --error-exitcode=1 --leak-check=full --suppressions=./test/Cond_jump.supp --track-origins=yes --show-leak-kinds=all ./$^

test_cov_%: test_%
	gcovr -b obj
	gcovr obj

test_all: $(TESTS)
	$(patsubst %,make test_% && ,$(MODULES)) true

test_cov_all: $(TESTS)
	make clean
	make test_all
	gcovr -b obj
	gcovr obj
	gcovr obj -x cov/cov.xml


$(DDIR)/inlupp2/%.o: $(DDIR)/inlupp2/%.c $(DDIR)/inlupp2/%.h $(DDIR)/inlupp2/data_structure.h $(DDIR)/inlupp2/iterator.h $(DDIR)/inlupp2/hash_table_internal.h $(DDIR)/inlupp2/linked_list_internal.h
	$(CC) $(FLAGS) $< -c -o $@

$(DDIR)/inlupp2/store: $(DDIR)/inlupp2/store.c $(OBJS) $(INLUPP2_OBJS)
	$(CC) $(FLAGS) $^ -o $@

demo: $(DDIR)/inlupp2/store
	valgrind --error-exitcode=1 --leak-check=full --suppressions=./test/Cond_jump.supp --track-origins=yes --show-leak-kinds=all ./$(DDIR)/inlupp2/store

inv_clear:
	make -C demo/inlupp2 inv_clear

stock: $(DDIR)/inlupp2/store
	make -C demo/inlupp2 stock
	valgrind --error-exitcode=1 --leak-check=full --suppressions=./test/Cond_jump.supp --track-origins=yes --show-leak-kinds=all ./demo/inlupp2/store < demo/inlupp2/Stock.txt

clean:
	rm -rf exe/*
	rm -rf obj/*
	rm -rf cov/*
	make -C demo/inlupp2 clean

.PHONY: clean test_% demo inv_clear

.PRECIOUS: $(ODIR)/%.o $(EDIR)/test_% $(EDIR)/%
