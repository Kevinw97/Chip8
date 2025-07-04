#ifndef CHIP8_AUDIO_H
#define CHIP8_AUDIO_H

class Chip8Audio
{
  SDL_AudioStream* stream;

public:
  int volume;
  int frequency;

  Chip8Audio();
  ~Chip8Audio();

  void PlayBeep();

};

#endif // CHIP8_H