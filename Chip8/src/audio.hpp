#ifndef CHIP8_AUDIO_H
#define CHIP8_AUDIO_H

class Chip8Audio
{
private:
  static SDL_AudioStream* stream;

  Chip8Audio();

  static int volume;
  static int frequency;

public:
  static Chip8Audio& get();

  void PlayBeep();
};

#endif // CHIP8_AUDIO_H