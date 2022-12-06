GC_files =  gc.c gc.h

gc: $(GC_files)
	gcc -Wall -g $(GC_files) -o gc.out

valgrind_gc: $(GC_files)
	valgrind --leak-check=full --track-origins=yes ./gc.out

clean:
	rm -rf *.out
