PROG_NAME=mandel.run	

all:
	g++ main.cpp -o ${PROG_NAME} -lsfml-graphics -lsfml-window -lsfml-system


clean:
	rm ${PROG_NAME}