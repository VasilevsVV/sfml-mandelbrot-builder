PROG_NAME=mandel.run

.DEFAULT_GOAL := all

main_loop_helper.o: main_loop_helper.h
	g++ -c main_loop_helper.cpp

all: clean main_loop_helper.o
	g++ main.cpp main_loop_helper.o renderer.h ctpl_stl.h big_int.cpp -o ${PROG_NAME} -pthread -lsfml-graphics -lsfml-window -lsfml-system

big_int-test.o: big_int.cpp big_int.h
	g++ -DBIG_INT_TESTS -c big_int.cpp -o big_int-test.o

test-big-int: big_int-test.o
	g++ big_int-test.o -o big_int-test
	./big_int-test

clean:
	rm -f ${PROG_NAME} main_loop_helper.o big_int-test.o big_int-test

.PHONY: clean test-big-int
