#include <stdio.h>
#include <stdlib.h>
#include "chip8_window.h"

static int chip8_window_thread_function(void *window);

SDL_Thread *chip8_window_thread;
struct chip8_window *chip8_window;

void chip8_window_initialise(struct chip8 *chip)
{
	SDL_Init(0);

	chip8_window = malloc(sizeof(struct chip8_window));

	chip8_window->chip = chip;
	chip8_window->quit = false;

	chip8_window_thread = SDL_CreateThread(
			chip8_window_thread_function, 
			"RenderingThread", 
			(void *)chip8_window);

	if (chip8_window_thread == NULL) {
		fprintf(stderr, "Error creating rendering thread: %s\n", SDL_GetError());
	}
}

void chip8_window_quit()
{
	chip8_window->quit = true;
	SDL_WaitThread(chip8_window_thread, NULL);
	SDL_Quit();
	chip8_window = NULL;
}

static int chip8_window_thread_function(void *window)
{
	int err;
	struct chip8_window *win = (struct chip8_window *)window;
	
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	win->window = SDL_CreateWindow(
			"CHIP-8",                  // window title
			SDL_WINDOWPOS_UNDEFINED,   // initial x position
			SDL_WINDOWPOS_UNDEFINED,   // initial y position
			640,                       // width, in pixels
			320,                       // height, in pixels
			SDL_WINDOW_OPENGL          // flags - see below
			);

	if (win->window == NULL) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		exit(1);
	}

	win->renderer = SDL_CreateRenderer(
			win->window,
			-1,
			SDL_RENDERER_ACCELERATED
			);

	if (win->renderer == NULL) {
		fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	win->texture = SDL_CreateTexture(
			win->renderer,
			SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STREAMING,
			CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT
			);

	if (win->texture == NULL) {
		fprintf(stderr, "Could not create texture: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_RenderSetLogicalSize(
			win->renderer,
			CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT
			);

	for (size_t i = 0; i < CHIP8_SCREEN_SIZE; i++) {
		win->buffer[i] = 0;
	}

	while (!(win->quit)) {
		for (size_t i = 0; i < CHIP8_SCREEN_SIZE; i++) {
			win->buffer[i] >>= 16;
			win->buffer[i] *= 0.5;
			win->buffer[i] |= (win->buffer[i] << 8) | (win->buffer[i] << 16);
			win->buffer[i] |= win->chip->gfx[i] * 0xFFFFFF;
		}
		err = SDL_UpdateTexture(
				win->texture, 
				NULL, 
				win->buffer, 
				CHIP8_SCREEN_WIDTH * sizeof(win->buffer[0])
				);
		if (err < 0) {
			fprintf(stderr, "Error updating texture: %s\n", SDL_GetError());
			exit(1);
		}

		err = SDL_RenderClear(win->renderer);
		if (err < 0) {
			fprintf(stderr, "Error clearing renderer: %s\n", SDL_GetError());
			if (win->renderer == NULL) {
				printf("NULL Renderer!\n");
			}
			exit(1);
		}

		err = SDL_RenderCopy(win->renderer, win->texture, NULL, NULL);
		if (err < 0) {
			fprintf(stderr, "Error copying texture: %s\n", SDL_GetError());
			exit(1);
		}

		SDL_RenderPresent(win->renderer);

		SDL_Delay(16);
	}
	return 0;
}

void chip8_window_redraw()
{
	for (size_t i = 0; i < CHIP8_SCREEN_SIZE; i++) {
		chip8_window->buffer[i] |= chip8_window->chip->gfx[i] * 0xFFFFFF;
	}
}
