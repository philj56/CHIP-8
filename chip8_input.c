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

// Returns key that changed, or -1 for a non-emulated key
int chip8_input_process(const SDL_Event *e);

void chip8_input_process_all(struct chip8 *chip)
{
	SDL_Event e;
	int key;
	while (SDL_PollEvent(&e) != 0) {
		key = chip8_input_process(&e);
		if (key < 0) {
			continue;
		}
		if (e.type == SDL_KEYDOWN) {
			chip->key[key] = 1;
		} else {
			chip->key[key] = 0;
		}
	}
}

int chip8_input_process(const SDL_Event *e)
{
	if (e->type == SDL_QUIT) {
		SDL_Quit();
		exit(0);
	} else if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP) {
		for (size_t i = 0; i < 16; i++) {
			if (e->key.keysym.sym == keymap[i]) {
				return i;
			}
		}
	}
	return -1;
}

int chip8_input_wait()
{
	SDL_Event e;
	int key;

	SDL_WaitEvent(&e);
	while ((key = chip8_input_process(&e)) < 0 || e.type != SDL_KEYDOWN)
	{
		SDL_WaitEvent(&e);
	}
	return key;
}
