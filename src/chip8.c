#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"
#include "chip8_window.h"
#include "chip8_audio.h"
#include "chip8_input.h"

const uint8_t chip8_fontset[80] =
{ 
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_initialise(struct chip8 *chip)
{
	chip->opcode = 0;

	// Clear display
	chip8_clear_screen(chip);

	// Clear stack
	memset(chip->stack, 0, CHIP8_STACK_SIZE * sizeof(chip->stack[0]));

	// Clear registers
	memset(chip->V, 0, CHIP8_REGISTER_SIZE * sizeof(chip->V[0]));
	chip->I = 0;
	chip->pc = CHIP8_PROGRAM_START;
	chip->sp = 0;

	// Clear keys
	memset(chip->key, 0, 16 * sizeof(chip->key[0]));

	// Clear memory
	memset(chip->memory, 0, CHIP8_MEMORY_SIZE * sizeof(chip->memory[0]));

	// Load fontset
	memcpy(chip->memory + CHIP8_FONTSET_START, chip8_fontset, 80);	

	// Reset timers
	chip->delay_timer = 0;
	chip->sound_timer = 0;	
	gettimeofday(&(chip->last_tick), NULL);
	gettimeofday(&(chip->time), NULL);

	// By default, don't use original bitshift operations
	chip->original_bitshift = false;
}

void chip8_load_rom(struct chip8* chip, const char *filename)
{
	size_t read;
	FILE *rom = fopen(filename, "r");
	if (rom == NULL)
	{
		fprintf(stderr, "Error opening file: %s\n", filename);
		exit(1);
	}
	read = fread(&(chip->memory[CHIP8_PROGRAM_START]), 1, CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_START, rom);
	fclose(rom);
	if (read == 0) {
		fprintf(stderr, "Error reading from file: %s\n", filename);
		exit(1);
	}
}

void chip8_clear_screen(struct chip8 *chip)
{
	memset(chip->gfx, 0, CHIP8_SCREEN_SIZE * sizeof(chip->gfx[0]));
}

void chip8_emulate_cycle(struct chip8 *chip)
{
	// Fetch opcode
	chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];
	chip->pc += 2;

	// Prefetch register numbers used in many operations
	uint8_t x = (chip->opcode & 0x0F00) >> 8;
	uint8_t y = (chip->opcode & 0x00F0) >> 4;

	// Decode opcode
	switch(chip->opcode & 0xF000)
	{
		case 0x0000:	
			switch(chip->opcode & 0x00FF)
			{	
				// 0x00E0: Clear the screen
				case 0x00E0:
					chip8_clear_screen(chip);
					break;

				// 0x00EE: Return
				case 0x00EE:
					--(chip->sp);
					chip->pc = chip->stack[chip->sp];
					break;

				// 0x0NNN: Call RCA 1802 at NNN
				default:
					fprintf(stderr, "Unimplemented opcode 0x%04X\n", chip->opcode);			
					break;
			}
			break;

		// 0x1NNN: Jump to address NNN
		case 0x1000:
			chip->pc = chip->opcode & 0x0FFF;
			break;

		// 0x2NNN: Call subroutine at NNN
		case 0x2000:
			if (chip->sp >= CHIP8_STACK_SIZE) {
				fprintf(stderr, "Stack overflow\n");
				exit(1);
			}
			chip->stack[chip->sp] = chip->pc;
			++(chip->sp);
			chip->pc = chip->opcode & 0x0FFF;
			break;

		// 0x3XNN: Skip next instruction if VX == NN 
		case 0x3000:
			if (chip->V[x] == (chip->opcode & 0x00FF)) {
				chip->pc += 2;
			}
			break;

		// 0x4XNN: Skip next instruction if VX != NN
		case 0x4000:
			if (chip->V[x] != (chip->opcode & 0x00FF)) {
				chip->pc += 2;
			}
			break;

		// 0x5XY0: Skip next instruction if VX == VY
		case 0x5000:
			if (chip->V[x] == chip->V[y]) {
				chip->pc += 2;
			}
			break;

		// 0x6XNN: Set VX = NN
		case 0x6000:
			chip->V[x] = chip->opcode & 0x00FF;
			break;

		// 0x7XNN: Set VX += NN
		case 0x7000:
			chip->V[x] += chip->opcode & 0x00FF;
			break;

		case 0x8000:
			switch(chip->opcode & 0x000F)
			{
				// 0x8XY0: Set VX = VY
				case 0x0000:
					chip->V[x] = chip->V[y];
					break;

				// 0x8XY1: Set VX |= VY
				case 0x0001:
					chip->V[x] |= chip->V[y];
					break;

				// 0x8XY2: Set VX &= VY
				case 0x0002:
					chip->V[x] &= chip->V[y];
					break;

				// 0x8XY3: Set VX ^= VY
				case 0x0003:
					chip->V[x] ^= chip->V[y];
					break;

				// 0x8XY4: Set VX += VY
				case 0x0004:
					chip->V[x] += chip->V[y];
					// Check for overflow
					if (chip->V[x] < chip->V[y]) {
						chip->V[0xF] = 1;
					} else {
						chip->V[0xF] = 0;
					}
					break;

				// 0x8XY5: Set VX -= VY
				case 0x0005:
					// Check if underflow will occur
					if (chip->V[x] < chip->V[y]) {
						chip->V[0xF] = 0;
					} else {
						chip->V[0xF] = 1;
					}
					chip->V[x] -= chip->V[y];
					break;

				// 0x8XY6: Set VX = (VY >>= 1)
				case 0x0006:
					if (chip->original_bitshift) {
						chip->V[0xF] = chip->V[y] & 0x01;
						chip->V[y] >>= 1;
						chip->V[x] = chip->V[y];
					} else {
						chip->V[0xF] = chip->V[x] & 0x01;
						chip->V[x] >>= 1;
					}
					break;

				// 0x8XY7: Set VX = VY - VX
				case 0x0007:
					// Check if underflow will occur
					if (chip->V[x] > chip->V[y]) {
						chip->V[0xF] = 0;
					} else {
						chip->V[0xF] = 1;
					}
					chip->V[x] = chip->V[y] - chip->V[x];
					break;

				// 0x8XYE: Set VX = (VY <<= 1)
				case 0x000E:
					if (chip->original_bitshift) {
						chip->V[0xF] = (chip->V[y] & 0x80) >> 7;
						chip->V[y] <<= 1;
						chip->V[x] = chip->V[y];
					} else {
						chip->V[0xF] = (chip->V[x] & 0x80) >> 7;
						chip->V[x] <<= 1;
					}
					break;

				default:
					fprintf(stderr, "Unknown opcode 0x%04X\n", chip->opcode);			
					break;
			}
			break;

		// 0x9XY0: Skip next instruction if VX != VY
		case 0x9000:
			if (chip->V[x] != chip->V[y]) {
				chip->pc += 2;
			}
			break;

		// 0xANNN: Sets I to address NNN
		case 0xA000:
			chip->I = chip->opcode & 0x0FFF;
			break;

		// 0xBNNN: Jump to address V0 + NNN
		case 0xB000:
			chip->pc = chip->V[0] + (chip->opcode & 0x0FFF);
			break;

		// 0xCXNN: Set VX = rand & NN
		case 0xC000:
			chip->V[x] = rand() & (chip->opcode & 0x00FF);
			break;

		// 0xDXYN: Draw 8xN sprite at (VX, VY)
		case 0xD000:
			chip8_window_draw_sprite(chip, chip->V[x], chip->V[y]);
			break;

		case 0xE000:
			switch(chip->opcode & 0x00FF)
			{
				// 0xEX9E: Skip next instruction if key VX is pressed
				case 0x009E:
					if (chip->key[chip->V[x]] != 0) {
						chip->pc += 2;
					}
					break;

				// 0xEXA1: Skip next instruction if key VX is not pressed
				case 0x00A1:
					if (chip->key[chip->V[x]] == 0) {
						chip->pc += 2;
					}
					break;

				default:
					fprintf(stderr, "Unknown opcode 0x%04X\n", chip->opcode);			
					break;
			}
			break;

		case 0xF000:
			switch(chip->opcode & 0x00FF)
			{
				// 0xFX07: Set VX to the delay timer
				case 0x0007:
					chip->V[x] = chip->delay_timer;
					break;

				// 0xFX0A: Wait for keypress, and store in VX
				case 0x000A:
					chip->V[x] = chip8_input_wait();
					break;

				// 0xFX15: Set the delay timer to VX
				case 0x0015:
					chip->delay_timer = chip->V[x];
					break;

				// 0xFX18: Set the sound timer to VX
				case 0x0018:
					chip->sound_timer = chip->V[x];
					break;

				// 0xFX1E: Set I += VX
				case 0x001E:
					chip->I += chip->V[x];
					// Check for overflow
					if (chip->I < chip->V[x] || chip->I > 0xFFF) {
						chip->V[0xF] = 1;
					} else {
						chip->V[0xF] = 0;
					}
					break;

				// 0xFX29: Sets I to the location of sprite VX
				case 0x0029:
					chip->I = CHIP8_FONTSET_START + 5 * chip->V[x];
					break;

				// 0xFX33: Store BCD(VX) at I
				case 0x0033:
					if (chip->I < CHIP8_MEMORY_SIZE + 3) {
						chip->memory[chip->I] = chip->V[x] / 100;
						chip->memory[chip->I + 1] = (chip->V[x] / 10) % 10;
						chip->memory[chip->I + 2] = (chip->V[x] % 100) % 10;
					}
					break;

				// 0xFX55: Stores from V0 to VX in memory at I
				case 0x0055:
					memcpy(chip->memory + chip->I, chip->V, x + 1);
					chip->I += x + 1;
					break;

				// 0xFX65: Loads V0 to VX from memory at I
				case 0x0065:
					memcpy(chip->V, chip->memory + chip->I, x + 1);
					chip->I += x + 1;
					break;

				default:
					fprintf(stderr, "Unknown opcode 0x%04X\n", chip->opcode);			
					break;
			}
			break;

		default:
			break;
	}
	gettimeofday(&(chip->time), NULL);
	uint64_t delta_ms = (chip->time.tv_sec - chip->last_tick.tv_sec) * 1000 
		+ (chip->time.tv_usec - chip->last_tick.tv_usec) / 1000;
	if (delta_ms > 16) {
		chip->last_tick = chip->time;
		if (chip->delay_timer > 0) {
			--chip->delay_timer;
		}
		if (chip->sound_timer > 0) {
			chip8_audio_on();
			--chip->sound_timer;
		} else {
			chip8_audio_off();
		}
	}
}

void chip8_print_state(struct chip8 *chip)
{
	printf("\nOpcode %04X\n", chip->opcode);
	printf("Registers:\n");
	for (unsigned int i = 0; i < CHIP8_REGISTER_SIZE; i++) {
		printf("\tV%01X: %u\n", i, chip->V[i]);
	}	
	printf("\tI:  %04X\n", chip->I);
	printf("\tpc: %04X\n", chip->pc);
	printf("\tsp: %u\n", chip->sp);

	printf("Timers:\n");
	printf("\tDelay: %u\n", chip->delay_timer);
	printf("\tSound: %u\n", chip->sound_timer);
}
