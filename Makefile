all: Run_test

Run_test: compile_test execute_test
Mem_check: run_valgrind

compile_test:
	g++ -Wall -std=c++17 -g -O3 -o test.o cache_lib.cc lru_evictor.cc cache_server.cc

run_valgrind:
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./test.o

execute_test:
	./test.o
	
clean:
	rm -f *.o
