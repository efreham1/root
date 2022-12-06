SDIR = src
EXEDIR = exe
ODIR = obj
TDIR = test
GC_files =  src/gc.c src/gc.h
CC = gcc

FLAGS = -I src -Wall -g

_OBJS = gc.o
OBJS = $(patsubst %, $(ODIR)/%, $(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c $(SDIR)/%.h
	$(CC) $(FLAGS) $< -c -o $@ 

$(EXEDIR)/gc: $(OBJS) 
	$(CC) $(FLAGS) $^ -o $@

valgrind_gc: $(EXEDIR)/gc
	valgrind --leak-check=full --track-origins=yes $^

$(EXEDIR)/test_%: $(TDIR)/test_%.c $(ODIR)/%.o
	$(CC) $(FLAGS) $^ -lcunit  -o $@

test_% : $(EXEDIR)/test_%
	valgrind --leak-check=full --track-origins=yes ./$^


clean:
	rm -rf exe/*
	rm -rf obj/*

.PHONY: clean test_%