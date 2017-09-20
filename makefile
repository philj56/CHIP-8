IDIR=include
ODIR=obj
SRCDIR=src
CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Werror -O2 -I$(IDIR)
LIBS=-lSDL2 -lm

_DEPS = chip8.h chip8_constants.h chip8_window.h chip8_input.h chip8_audio.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = chip8.o chip8_window.o chip8_input.o chip8_audio.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJ): | $(ODIR)

$(ODIR):
	mkdir -p $@


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(SRCDIR)/*~ chip8 $(IDIR)/*~
