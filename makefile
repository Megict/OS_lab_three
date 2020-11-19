
all: main.c child_any.c
	gcc --std=c99 main.c -o exethr.out
	gcc --std=c99 child_any.c -o child_any.out
