all: Run_test

Run_test: compile_test execute_test
Mem_check: run_valgrind

compile_test:
	c++ -Wall -std=c++17 -O3 -o test.o cache_lib.cc lru_evictor.cc simple_server.cc

run_valgrind:
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./test.o

execute_test:
	./test.o
	
clean:
	rm -f *.o
