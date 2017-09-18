#include <stdio.h>
#include <stdlib.h>
#include "chip8_window.h"

void chip8_window_initialise(struct chip8_window *win)
{
	win->window = SDL_CreateWindow(
			"CHIP-8",                  // window title
			SDL_WINDOWPOS_UNDEFINED,   // initial x position
			SDL_WINDOWPOS_UNDEFINED,   // initial y position
			640,                       // width, in pixels
			320,                       // height, in pixels
			SDL_WINDOW_OPENGL          // flags - see below
			);

	win->renderer = SDL_CreateRenderer(win->window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(win->renderer, CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT);

	win->texture = SDL_CreateTexture(win->renderer,
			SDL_PIXELFORMAT_RGB444,
			SDL_TEXTUREACCESS_STREAMING,
			CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT);

	// Check that the window was successfully created
	if (win->window == NULL) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		exit(1);
	}
}

void chip8_window_draw(struct chip8_window *win, struct chip8 *chip)
{
	for (size_t i = 0; i < CHIP8_SCREEN_SIZE; i++) {
		win->buffer[i] = chip->gfx[i] * 0xFFFFFF;
	}

	SDL_UpdateTexture(win->texture, NULL, win->buffer, CHIP8_SCREEN_WIDTH*sizeof(win->buffer[0]));

	SDL_RenderClear(win->renderer);
	SDL_RenderCopy(win->renderer, win->texture, NULL, NULL);
	SDL_RenderPresent(win->renderer);
}
