#include "chip8.h"
#include "chip8_input.h"

const SDL_Keycode keymap[16] = {
	SDLK_x,	// 0
	SDLK_1,	// 1
	SDLK_2,	// 2
	SDLK_3,	// 3
	SDLK_q,	// 4
	SDLK_w,	// 5
	SDLK_e,	// 6
	SDLK_a,	// 7
	SDLK_s,	// 8
	SDLK_d,	// 9
	SDLK_z,	// A
	SDLK_c,	// B
	SDLK_4,	// C
	SDLK_r,	// D
	SDLK_f,	// E
	SDLK_v	// F
};

void chip8_input_process(struct chip8 *chip, SDL_Event *e);

void chip8_input_process_all(struct chip8 *chip)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		chip8_input_process(chip, &e);
	}
}

void chip8_input_process(struct chip8 *chip, SDL_Event *e)
{
	if (e->type == SDL_QUIT) {
		SDL_Quit();
		exit(0);
	} else {
		if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP) {
			for (size_t i = 0; i < 16; i++) {
				if (e->key.keysym.sym == keymap[i]) {
					if (e->type == SDL_KEYDOWN) {
						chip->key[i] = 1;
					} else {
						chip->key[i] = 0;
					}
					break;
				}
			}
		}
	}
}

void chip8_input_wait(struct chip8 *chip)
{
	SDL_Event e;
	SDL_WaitEvent(&e);
	chip8_input_process(chip, &e);
}
