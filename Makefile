CC = gcc
LD = ld

prog: main.c tuibb.c
	# todo: use libs from third_party
	$(CC) -o prog main.c tuibb.c -std=c11 -ltermbox -lrt -lllist -lpthread -D_POSIX_C_SOURCE=199309L -D__STDC_WANT_LIB_EXT1__=1
	./prog

