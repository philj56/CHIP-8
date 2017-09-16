#include "chip8.h"

int main()
{
        struct chip8 chip;

	chip8_initialise(&chip);
	chip8_emulate_cycle(&chip);
        return 0;
}
