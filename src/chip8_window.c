#include <stdio.h>
#include <stdlib.h>
#include "chip8_window.h"
#include "filters/filters.h"

static int chip8_window_thread_function(void *window);

SDL_Thread *chip8_window_thread;
struct chip8_window chip8_window;

void chip8_window_initialise(struct chip8 *chip)
{
	SDL_Init(0);

	chip8_window.chip = chip;
	chip8_window.quit = false;

	chip8_window.sync = SDL_CreateMutex();
	if (chip8_window.sync == NULL) {
		fprintf(stderr, "Could not create mutex: %s\n", SDL_GetError());
		exit(1);
	}

	chip8_window_thread = SDL_CreateThread(
			chip8_window_thread_function, 
			"RenderingThread", 
			(void *)(&chip8_window));

	if (chip8_window_thread == NULL) {
		fprintf(stderr, "Error creating rendering thread: %s\n", SDL_GetError());
		exit(1);
	}
}

void chip8_window_quit()
{
	chip8_window.quit = true;
	SDL_WaitThread(chip8_window_thread, NULL);
	SDL_DestroyMutex(chip8_window.sync);
	SDL_DestroyTexture(chip8_window.texture);
	SDL_DestroyRenderer(chip8_window.renderer);
	SDL_DestroyWindow(chip8_window.window);
	SDL_Quit();
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
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE          // flags - see below
			);

	if (win->window == NULL) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		SDL_DestroyMutex(win->sync);
		SDL_Quit();
		exit(1);
	}

	win->renderer = SDL_CreateRenderer(
			win->window,
			-1,
			SDL_RENDERER_ACCELERATED
			);

	if (win->renderer == NULL) {
		fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(win->window);
		SDL_DestroyMutex(win->sync);
		SDL_Quit();
		exit(1);
	}

	win->texture = SDL_CreateTexture(
			win->renderer,
			SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STREAMING,
			CHIP8_SCREEN_WIDTH * 3, CHIP8_SCREEN_HEIGHT * 3
			);

	if (win->texture == NULL) {
		fprintf(stderr, "Could not create texture: %s\n", SDL_GetError());
		SDL_DestroyRenderer(win->renderer);
		SDL_DestroyWindow(win->window);
		SDL_DestroyMutex(win->sync);
		SDL_Quit();
		exit(1);
	}

	SDL_RenderSetLogicalSize(
			win->renderer,
			CHIP8_SCREEN_WIDTH * 3, CHIP8_SCREEN_HEIGHT * 3
			);

	for (size_t i = 0; i < CHIP8_SCREEN_SIZE; i++) {
		win->buffer[i] = 0;
	}

	atexit(chip8_window_quit);

	while (!(win->quit)) {
		if (SDL_LockMutex(win->sync) < 0) {
			fprintf(stderr, "Could not lock mutex: %s\n", SDL_GetError());
		}
		for (size_t i = 0; i < CHIP8_SCREEN_SIZE; i++) {
			win->buffer[i] >>= 16;
			win->buffer[i] *= 0.7;
			win->buffer[i] |= (win->buffer[i] << 8) | (win->buffer[i] << 16);
			win->buffer[i] |= win->chip->gfx[i] * 0xFFFFFF;
		}
		scale3x(win->buffer2x, win->buffer, CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT);
		if (SDL_UnlockMutex(win->sync) < 0) {
			fprintf(stderr, "Could not unlock mutex: %s\n", SDL_GetError());
		}
		err = SDL_UpdateTexture(
				win->texture, 
				NULL, 
				win->buffer2x, 
				3 * CHIP8_SCREEN_WIDTH * sizeof(win->buffer2x[0])
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

void chip8_window_draw_sprite(struct chip8 *chip, uint8_t x, uint8_t y)
{
	uint16_t height = chip->opcode & 0x000F;
	uint8_t sprite_row;
	uint8_t px;
	uint8_t py;
	uint16_t pixel;
	uint8_t pv;

	x %= CHIP8_SCREEN_WIDTH;
	y %= CHIP8_SCREEN_HEIGHT;

	chip->V[0xF] = 0;
	if (SDL_LockMutex(chip8_window.sync) < 0) {
		fprintf(stderr, "Could not lock mutex: %s\n", SDL_GetError());
	}
	for (uint8_t yline = 0; yline < height; ++yline) {
		if (chip->I + yline > CHIP8_MEMORY_SIZE) {
			break;
		}
		sprite_row = chip->memory[chip->I + yline];
		py = y + yline;
		if (py >= CHIP8_SCREEN_HEIGHT) {
			break;
		}
		for (uint8_t xline = 0; xline < 8; ++xline) {
			px = x + xline;
			if (px >= CHIP8_SCREEN_WIDTH) {
				break;
			}
			pv = (sprite_row & (0x80 >> xline)) >> (7 - xline);
			pixel = (px + py * CHIP8_SCREEN_WIDTH);
			chip->V[0xF] |= (chip->gfx[pixel] & pv);
			chip->gfx[pixel] ^= pv;
		}
	}
	if (SDL_UnlockMutex(chip8_window.sync) < 0) {
		fprintf(stderr, "Could not unlock mutex: %s\n", SDL_GetError());
	}
}
