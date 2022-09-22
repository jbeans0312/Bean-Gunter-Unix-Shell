CC = gcc
CFLAGS=-I.

ushell: ushell.o
	$(CC) -o ush ushell.o
