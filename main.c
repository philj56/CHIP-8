#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "chip8_window.h"

int main()
{
	struct chip8 chip;
	struct chip8_window win;
	SDL_Event e;

	chip8_initialise(&chip);

	chip8_load_rom(&chip, "roms/UFO");
	
	SDL_Init(SDL_INIT_VIDEO);
	chip8_window_initialise(&win);

	while(true) {
//		printf("%u\n", chip.pc);
		while(SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT) {
				SDL_Quit();
				exit(0);
			}
		}
		chip8_emulate_cycle(&chip);
		if(chip.redraw) {
			chip8_window_draw(&win, &chip);
			SDL_Delay(16);
		}
	}

	return 0;
}
