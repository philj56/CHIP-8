#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "chip8_window.h"

int main(int argc, char **argv)
{
	if (argc == 1) {
		printf("Usage: %s rom_file\n", argv[0]);
		exit(1);
	}
	struct chip8 chip;
	struct chip8_window win;
	SDL_Event e;

	chip8_initialise(&chip);

	chip8_load_rom(&chip, argv[1]);

	SDL_Init(SDL_INIT_VIDEO);
	chip8_window_initialise(&win);

	while (true) {
		//printf("%04X\n", chip.opcode);
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				SDL_Quit();
				exit(0);
			}
		}
		chip8_emulate_cycle(&chip);
		if (chip.redraw) {
			chip8_window_draw(&win, &chip);
		}
		
		SDL_Delay(16);
		//chip8_print_state(&chip);
	}

	return 0;
}
