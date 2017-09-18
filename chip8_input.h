#ifndef CHIP8_INPUT_H
#define CHIP8_INPUT_H

#include <SDL2/SDL.h>

void chip8_input_process_all(struct chip8 *chip);
void chip8_input_wait(struct chip8 *chip);

#endif /* CHIP8_INPUT_H */