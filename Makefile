CC = gcc
LD = ld

prog: main.c
	# todo: use termbox lib from third_party
	$(CC) -o prog main.c -std=c11 -ltermbox
	./prog

