#include <SDL3/SDL.h>
#include "audio.hpp"

Chip8Audio::Chip8Audio()
{
  SDL_AudioSpec audio_spec;
  audio_spec.format = SDL_AUDIO_S32BE;
  audio_spec.channels = 1;
  audio_spec.freq = 44100;
  
  // Default volume to 10 and frequency to 440 Hz tone
  volume = 10;
  frequency = 440;

  stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, NULL, NULL);
  SDL_ResumeAudioStreamDevice(stream);
}

void Chip8Audio::PlayBeep()
{
  static int samples[1024];

  int current_sample_index = 0;
  for (int i = 0; i < SDL_arraysize(samples); ++i)
  {
    int samples_per_wave = 44100 / frequency; // Samples/Time divided by Waves/Time gives us Samples/Wave, essentially telling us how many samples are in each wave.
    int half_samples_per_wave = samples_per_wave / 2;
    samples[i] = ((current_sample_index++ / half_samples_per_wave) % 2) ? volume : -volume;
    current_sample_index %= samples_per_wave;
  }

  SDL_PutAudioStreamData(stream, samples, sizeof(samples));
  return;
}

Chip8Audio::~Chip8Audio() {}