CC = gcc
CFLAGS=-I.
DEPS = shell.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ushell: ushell.o path.o
	$(CC) -o ush ushell.o path.o

.PHONY: clean

clean:
	rm *.o ush
