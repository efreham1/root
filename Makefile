SDIR = src
EDIR = exe
ODIR = obj
TDIR = test

CC = gcc

FLAGS = -I src -Wall -g -fprofile-arcs -ftest-coverage

MODULES = gc

OBJS = $(patsubst %,$(ODIR)/%.o,$(MODULES))
TESTS = $(patsubst %,$(EDIR)/test_%,$(MODULES))

$(ODIR)/%.o: $(SDIR)/%.c $(SDIR)/%.h
	$(CC) $(FLAGS) $< -c -o $@ 

$(EDIR)/%: $(OBJS) 
	$(CC) $(FLAGS) $^ -o $@

$(EDIR)/test_%: $(TDIR)/test_%.c $(ODIR)/%.o
	$(CC) $(FLAGS) $^ -lcunit  -o $@

test_%: $(EDIR)/test_%
	valgrind --leak-check=full --track-origins=yes ./$^

test_cov_%: test_%
	gcovr -b obj
	gcovr obj

test_all: $(TESTS)
	$(patsubst %,valgrind --leak-check=full --track-origins=yes ./% && ,$(TESTS)) true

test_cov_all: $(TESTS)
	make test_all
	gcovr -b obj
	gcovr obj
	gcovr obj -x cov/cov.xml
clean:
	rm -rf exe/*
	rm -rf obj/*

.PHONY: clean test_%

.PRECIOUS: $(ODIR)/%.o $(EDIR)/test_% $(EDIR)/%
