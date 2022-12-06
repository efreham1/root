SDIR = src
EXEDIR = exe
ODIR = obj
TDIR = test
GC_files =  src/gc.c src/gc.h
CC = gcc

FLAGS = -Wall -g

_OBJS = gc.o
OBJS = $(patsubst %, $(ODIR)/%, $(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c $(SDIR)/%.h
	$(CC) $(FLAGS) $< -c -o $@ 

$(EXEDIR)/gc: $(OBJS) 
	$(CC) $(FLAGS) $^ -o $@

valgrind_gc: $(EXEDIR)/gc
	valgrind --leak-check=full --track-origins=yes $^

$(TDIR)/test_%: $(ODIR)/%.o $(TDIR)/test_%.c
	$(CC) $(FLAGS) $^ -lcunit  -o $@

clean:
	rm -rf exe/*
	rm -rf obj/*

.PHONY: clean test