PROG_NAME=mandel.run	

.DEFAULT_GOAL := all

main_loop_helper.o: main_loop_helper.h
	g++ -c main_loop_helper.cpp

all: clean main_loop_helper.o
	g++ main.cpp main_loop_helper.o renderer.h ctpl_stl.h -o ${PROG_NAME} -pthread -lsfml-graphics -lsfml-window -lsfml-system


clean:
	rm -f ${PROG_NAME} main_loop_helper.o