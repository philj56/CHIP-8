#ifndef CHIP8_WINDOW_H
#define CHIP8_WINDOW_H

#include <SDL2/SDL.h>
#include "chip8.h"
#include "chip8_constants.h"

struct chip8_window {
	struct chip8 *chip;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_mutex *sync;
	uint32_t buffer[CHIP8_SCREEN_SIZE];
	uint32_t buffer2x[9 * CHIP8_SCREEN_SIZE];
	bool quit;
};

void chip8_window_initialise(struct chip8 *chip);
void chip8_window_quit();
void chip8_window_draw_sprite(struct chip8 *chip, uint8_t x, uint8_t y);


#endif /* CHIP8_WINDOW_H */
