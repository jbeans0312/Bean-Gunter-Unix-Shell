CC = gcc
CFLAGS=-I.
DEPS = shell.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ushell: ushell.o
	$(CC) -o ush ushell.o

.PHONY: clean

clean:
	rm *.o ush
