#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include "chip8_constants.h"

struct chip8 {
	// Memory 
	uint8_t memory[CHIP8_MEMORY_SIZE];
	uint16_t stack[CHIP8_STACK_SIZE];

	// CPU
	uint8_t V[CHIP8_REGISTER_SIZE];	// General registers
	uint16_t I;		// Index register
	uint16_t pc;		// Program counter
	uint16_t sp;		// Stack pointer
	uint16_t opcode;	// Current opcode
	uint8_t delay_timer;
	uint8_t sound_timer;

	// IO
	uint8_t gfx[CHIP8_SCREEN_SIZE];	// Screen buffer
	uint8_t key[16];	// Keypad

	// Flags
	bool redraw;

	// Internal timer
	struct timeval last_tick;
	struct timeval time;
};

// Wipe all registers and memory
void chip8_initialise(struct chip8* chip);

// Load a rom into memory
void chip8_load_rom(struct chip8* chip, const char *filename);

// Clear the screen buffer
void chip8_clear_screen(struct chip8 *chip);

// Step one cycle
void chip8_emulate_cycle(struct chip8* chip);

// Print the internal state of the emulator
void chip8_print_state(struct chip8 *chip);

#endif /* CHIP8_H */
