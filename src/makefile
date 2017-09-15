CC = mpic++

all: remove main

main:
	${CC} Agent.cpp main.cpp -o main

run:
	mpirun -n 4 main
	
remove:
	rm -f main
