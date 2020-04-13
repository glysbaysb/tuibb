CC = gcc
LD = ld

prog: main.c
	# todo: use termbox lib from third_party
	$(CC) -o prog main.c -std=c11 -ltermbox -lrt -D_POSIX_C_SOURCE=199309L
	./prog

