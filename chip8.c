#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"

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

	// Clear memory
	memset(chip->memory, 0, CHIP8_MEMORY_SIZE * sizeof(chip->memory[0]));

	// Load fontset
	// TODO: Implement

	// Reset timers
	chip->delay_timer = 0;
	chip->sound_timer = 0;	

	chip->redraw = true;
}

void chip8_load_rom(struct chip8* chip, const char *filename)
{
	FILE *rom = fopen(filename, "r");
	fread(&(chip->memory[CHIP8_PROGRAM_START]), 1, CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_START, rom);
	fclose(rom);
}

void chip8_clear_screen(struct chip8 *chip)
{
	memset(chip->gfx, 0, CHIP8_SCREEN_SIZE * sizeof(chip->gfx[0]));
}

void chip8_emulate_cycle(struct chip8 *chip)
{
	// Don't redraw unless necessary
	chip->redraw = false;

	// Fetch opcode
	chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];
	chip->pc += 2;
	
	// Prefetch register numbers used in many operations
	uint8_t x = chip->opcode & 0x0F00 >> 2;
	uint8_t y = chip->opcode & 0x00F0 >> 1;

	// Decode opcode
	switch(chip->opcode & 0xF000)
	{
		case 0x0000:	
			switch(chip->opcode & 0x00FF)
			{	
				// 0x00E0: Clear the screen
				case 0x00E0:
					chip8_clear_screen(chip);
					chip->redraw = true;
					break;
					
				// 0x00EE: Return
				case 0x00EE:
					--(chip->sp);
					chip->pc = chip->stack[chip->sp];
					break;
					
				// 0x0NNN: Call RCA 1802 at NNN
				default:
					// TODO: Implement
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
					if(chip->V[x] < chip->V[y]) {
						chip->V[0xF] = 1;
					} else {
						chip->V[0xF] = 0;
					}
					break;

				// 0x8XY5: Set VX -= VY
				case 0x0005:
					// Check if underflow will occur
					if(chip->V[x] < chip->V[y]) {
						chip->V[0xF] = 0;
					} else {
						chip->V[0xF] = 1;
					}
					chip->V[x] -= chip->V[y];
					break;

				// 0x8XY6: Set VX = (VY >>= 1)
				case 0x0006:
					chip->V[0xF] = chip->V[y] & 0x000F;
					chip->V[y] >>= 1;
					chip->V[x] = chip->V[y];
					break;

				// 0x8XY7: Set VX = VY - VX
				case 0x0007:
					// Check if underflow will occur
					if(chip->V[x] > chip->V[y]) {
						chip->V[0xF] = 0;
					} else {
						chip->V[0xF] = 1;
					}
					chip->V[x] = chip->V[y] - chip->V[x];
					break;

				// 0x8XYE: Set VX = (VY <<= 1)
				case 0x000E:
					chip->V[0xF] = (chip->V[y] & (uint8_t)0xF000) >> 3;
					chip->V[y] <<= 1;
					chip->V[x] = chip->V[y];
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
			chip->I = (chip->opcode & 0x0FFF);
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
			{
				uint16_t height = chip->opcode & 0x000F;
				uint16_t pixel;

				chip->V[0xF] = 0;
				for(uint16_t yline = 0; yline < height; ++yline) {
					pixel = chip->memory[chip->I + yline];
					for(uint16_t xline = 0; xline < 8; ++xline) {
						if((pixel & (0x80 >> xline)) != 0) {
							if(chip->gfx[(x + xline + ((y + yline) * CHIP8_SCREEN_WIDTH))] == 1) {
								chip->V[0xf] = 1;
							}
							chip->gfx[(x + xline + ((y + yline) * CHIP8_SCREEN_WIDTH))] ^= 1;
						}
					}
				}
			}
			chip->redraw = true;
			break;
			
		case 0xE000:
			switch(chip->opcode & 0x00FF)
			{
				// 0xEX9E: Skip next instruction if key VX is pressed
				case 0x009E:
					// TODO: Implement
					fprintf(stderr, "Unimplemented opcode 0x%04X\n", chip->opcode);			
					break;
				
				// 0xEXA1: Skip next instruction if key VX is not pressed
				case 0x00A1:
					// TODO: Implement
					fprintf(stderr, "Unimplemented opcode 0x%04X\n", chip->opcode);			
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
					// TODO: Implement
					fprintf(stderr, "Unimplemented opcode 0x%04X\n", chip->opcode);			
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
					if(chip->I < chip->V[x] || chip->I > 0xFFF) {
						chip->V[0xF] = 1;
					} else {
						chip->V[0xF] = 0;
					}
					break;

				// 0xFX29: Sets I to the location of sprite VX
				case 0x0029:
					// TODO: Implement
					fprintf(stderr, "Unimplemented opcode 0x%04X\n", chip->opcode);			
					break;

				// 0xFX33: Store BCD(VX) at I
				case 0x0033:
					// TODO: Implement
					fprintf(stderr, "Unimplemented opcode 0x%04X\n", chip->opcode);			
					break;

				// 0xFX55: Stores from V0 to VX in memory at I
				case 0x0055:
					for(uint8_t i = 0; i <= x; i++) {
						chip->memory[chip->I] = chip->V[i];
						++(chip->I);
					}
					break;

				// 0xFX65: Loads V0 to VX from memory at I
				case 0x0065:
					for(uint8_t i = 0; i <= x; i++) {
						chip->V[i] = chip->memory[chip->I];
						++(chip->I);
					}
					break;

				default:
					fprintf(stderr, "Unknown opcode 0x%04X\n", chip->opcode);			
					break;
			}
			break;

		default:
			break;
	}
}

