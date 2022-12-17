SDIR = src
EDIR = exe
ODIR = obj
TDIR = test

CC = gcc

FLAGS = -I src -Wall -g -fprofile-arcs -ftest-coverage

MODULES = gc page heap metadata mover

OBJS = $(patsubst %,$(ODIR)/%.o,$(MODULES))
TESTS = $(patsubst %,$(EDIR)/test_%,$(MODULES))

$(ODIR)/%.o: $(SDIR)/%.c $(SDIR)/%.h $(SDIR)/structs.h
	$(CC) $(FLAGS) $< -c -o $@ 

$(EDIR)/%: $(SDIR)/%.c $(OBJS) 
	$(CC) $(FLAGS) $^ -o $@

$(EDIR)/test_%: $(TDIR)/test_%.c $(OBJS)
	$(CC) $(FLAGS) $^ -lcunit  -o $@

test_%: $(EDIR)/test_%
	valgrind --error-exitcode=1 --leak-check=full --track-origins=yes ./$^

test_cov_%: test_%
	gcovr -b obj
	gcovr obj

test_all: $(TESTS)
	$(patsubst %,make test_% && ,$(MODULES)) true

test_cov_all: $(TESTS)
	make test_all
	gcovr -b obj
	gcovr obj
	gcovr obj -x cov/cov.xml
clean:
	rm -rf exe/*
	rm -rf obj/*
	rm -rf cov/*

.PHONY: clean test_%

.PRECIOUS: $(ODIR)/%.o $(EDIR)/test_% $(EDIR)/%
