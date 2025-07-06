#ifndef CHIP8_H
#define CHIP8_H

#define CHIP8_SOUND_TIMER_NONZERO 0x1
#define CHIP8_DELAY_TIMER_NONZERO 0x2
#define CHIP8_ALL_TIMERS_ZERO 0x0

class Chip8
{
  unsigned char memory[4096];
  unsigned char V[16];

  unsigned short I;
  unsigned short pc;

  unsigned char delay_timer;
  unsigned char sound_timer;

  unsigned short stack[16];
  unsigned short sp;

  void reset();

public:
  unsigned char graphics[64 * 32]; // Width * Height : 64 * 32
  unsigned char render_flag; // Currently not used since display constantly renders at 60 Hz

  unsigned char keys[16];

  Chip8();
  ~Chip8();

  int load(const char* file_path);
  void emulate_cycle();
  int tick_timers();

};

#endif // CHIP8_H