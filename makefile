PROG_NAME=mandel.run	

all:
	g++ main.cpp renderer.h -o ${PROG_NAME} -pthread -lsfml-graphics -lsfml-window -lsfml-system


clean:
	rm ${PROG_NAME}