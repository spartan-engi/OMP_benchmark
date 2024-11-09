build:
	gcc main.c -o main.exe -fopenmp -lm -g

run:
	./main.exe

bench:
	./main.exe 1