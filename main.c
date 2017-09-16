#include <stdio.h>
#include <stdbool.h>
#include "chip8.h"

int main()
{
	struct chip8 chip;

	chip8_initialise(&chip);

	chip8_load_rom(&chip, "roms/PONG");

	while(true) {
		printf("%u\n", chip.pc);
		chip8_emulate_cycle(&chip);
	}

	return 0;
}
