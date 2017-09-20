#include <math.h>
#include <SDL2/SDL.h>
#include "chip8_audio.h"

#define CHIP8_AUDIO_AMPLITUDE 255
#define CHIP8_AUDIO_SAMPLE_RATE 48000
#define CHIP8_AUDIO_BEEP_SAMPLES 1000
#define CHIP8_AUDIO_BEEP_FREQUENCY 441

SDL_AudioDeviceID dev;
uint8_t buffer[CHIP8_AUDIO_BEEP_SAMPLES];

void chip8_audio_initialise()
{
	SDL_AudioSpec spec;

	SDL_zero(spec);
	spec.freq = CHIP8_AUDIO_SAMPLE_RATE;
	spec.format = AUDIO_U8;
	spec.channels = 1;
	spec.samples = 4096;
	spec.callback = NULL;
	
	SDL_InitSubSystem(SDL_INIT_AUDIO);
	
	dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (dev == 0) {
		fprintf(stderr, "Failed to open audio: %s", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	
	for (uint64_t i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++) {
		double time = (double) i / CHIP8_AUDIO_SAMPLE_RATE;
		buffer[i] = (uint8_t)(CHIP8_AUDIO_AMPLITUDE * (sin(2.0 * M_PI * CHIP8_AUDIO_BEEP_FREQUENCY * time)));
	}
	
	SDL_PauseAudioDevice(dev, 0);
}

void chip8_audio_beep()
{
	int err;
	
	err = SDL_QueueAudio(dev, buffer, sizeof(buffer));
	if (err < 0) {
		fprintf(stderr, "Failed to queue audio: %s", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
}
