CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O2
DEPS = chip8.h
OBJ = chip8.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ chip8
