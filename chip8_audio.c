#include <math.h>
#include <SDL2/SDL.h>
#include "chip8_audio.h"

#define CHIP8_AUDIO_AMPLITUDE 255
#define CHIP8_AUDIO_SAMPLE_RATE 48000
#define CHIP8_AUDIO_BEEP_FREQUENCY 300

SDL_AudioDeviceID dev;
uint64_t n;

void chip8_audio_callback(void *userdata, uint8_t *buffer, int length);

void chip8_audio_initialise()
{
	SDL_AudioSpec spec;
	n = 0;

	SDL_zero(spec);
	spec.freq = CHIP8_AUDIO_SAMPLE_RATE;
	spec.format = AUDIO_U8;
	spec.channels = 1;
	spec.samples = 512;
	spec.callback = chip8_audio_callback;
	spec.userdata = &n;
	
	SDL_InitSubSystem(SDL_INIT_AUDIO);
	
	dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (dev == 0) {
		fprintf(stderr, "Failed to open audio: %s", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
}

void chip8_audio_on()
{
	SDL_PauseAudioDevice(dev, 0);
}

void chip8_audio_off()
{
	SDL_PauseAudioDevice(dev, 1);
}

void chip8_audio_callback(void *userdata, uint8_t *buffer, int length)
{
	uint64_t *n_samples = (uint64_t *)userdata;
	for (int i = 0; i < length; i++, (*n_samples)++) {
		double time = (double) (*n_samples) / CHIP8_AUDIO_SAMPLE_RATE;
		buffer[i] = (uint8_t)(CHIP8_AUDIO_AMPLITUDE * (sin(2.0 * M_PI * CHIP8_AUDIO_BEEP_FREQUENCY * time)));
	}
}
