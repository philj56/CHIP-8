CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Werror -O2
DEPS = chip8.h chip8_constants.h chip8_window.h chip8_input.h
OBJ = chip8.o chip8_window.o chip8_input.o main.o
LIBS=-lSDL2

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ chip8
