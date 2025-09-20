#include <SDL3/SDL.h>
#include "audio.hpp"
#include <iostream>
#include <fstream>

// Default volume to 10 and frequency to 440 Hz tone
int Chip8Audio::volume = 10;
int Chip8Audio::frequency = 440;

SDL_AudioStream* Chip8Audio::stream;

Chip8Audio& Chip8Audio::get()
{
  static Chip8Audio instance;
  return instance;
}

Chip8Audio::Chip8Audio()
{
  SDL_AudioSpec audio_spec;
  audio_spec.format = SDL_AUDIO_S32BE;
  audio_spec.channels = 1;
  audio_spec.freq = 44100;

  stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, NULL, NULL);
  SDL_ResumeAudioStreamDevice(stream);
}

void Chip8Audio::PlayBeep()
{
  static int samples[1024];
  static bool computed = false;

  int current_sample_index = 0;
  if (!computed)
  {
    for (int i = 0; i < SDL_arraysize(samples); ++i)
    {
      int samples_per_wave = 44100 / frequency; // Samples/Time divided by Waves/Time gives us Samples/Wave, essentially telling us how many samples are in each wave.
      int half_samples_per_wave = samples_per_wave / 2;
      samples[i] = ((current_sample_index++ / half_samples_per_wave) % 2) ? volume : -volume;
      current_sample_index %= samples_per_wave;
    }
    computed = true;
  }

  SDL_PutAudioStreamData(stream, samples, sizeof(samples));
  return;
}
