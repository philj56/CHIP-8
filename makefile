CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Werror -O2 -g
DEPS = chip8.h chip8_constants.h chip8_window.h chip8_input.h chip8_audio.h
OBJ = chip8.o chip8_window.o chip8_input.o chip8_audio.o main.o
LIBS=-lSDL2 -lm

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ chip8
