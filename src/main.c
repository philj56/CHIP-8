#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "chip8_window.h"
#include "chip8_input.h"
#include "chip8_audio.h"

int main(int argc, char **argv)
{
	struct timespec clock_time = {.tv_sec = 0, .tv_nsec = 2000000};
	if (argc == 1) {
		printf("Usage: %s rom_file\n", argv[0]);
		exit(1);
	}
	struct chip8 chip;

	srand(time(NULL));
	chip8_initialise(&chip);

	chip8_load_rom(&chip, argv[1]);

	chip8_window_initialise(&chip);

	chip8_audio_initialise();

	while (true) {
		chip8_input_process_all(&chip);

		chip8_emulate_cycle(&chip);
		//printf("%04X\n", chip.opcode);
		//chip8_print_state(&chip);

		nanosleep(&clock_time, NULL);
	}

	return 0;
}
