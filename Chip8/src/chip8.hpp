#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <functional>
#include <unordered_map>

#define CHIP8_SOUND_TIMER_NONZERO 0x1
#define CHIP8_DELAY_TIMER_NONZERO 0x2
#define CHIP8_ALL_TIMERS_ZERO 0x0

using opcode_function = std::function<void(unsigned short, unsigned char, unsigned char, unsigned char)>;

class Chip8
{
private:
  static std::array<unsigned char, 4096> memory;
  static std::array<unsigned char, 16> V;
  static std::array<unsigned short, 16> stack;
  
  static unsigned short I;
  static unsigned short pc;
  static unsigned short sp;

  static unsigned char delay_timer;
  static unsigned char sound_timer;
  
  Chip8();

  static void reset();

  static void op_default(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_0nnn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_00e0(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_00ee(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_1nnn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_2nnn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_3xkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_4xkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_5xy0(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_6xkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_7xkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_8xy0(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_8xy1(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_8xy2(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_8xy3(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_8xy4(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_8xy5(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_8xy6(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_8xy7(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_8xye(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_9xy0(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_annn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_bnnn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_cxkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_dxyn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_ex9e(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_exa1(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_fx07(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_fx0a(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_fx15(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_fx18(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_fx1e(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_fx29(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_fx33(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_fx55(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);
  static void op_fx65(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val);

  static opcode_function get_function(unsigned short opcode);


public:
  static std::array<unsigned char, 64 * 32> graphics;
  static std::array<unsigned char, 16> keys;

  static unsigned char render_flag; // Currently not used since display constantly renders at 60 Hz

  static Chip8& get();

  static int load(const char* file_path);
  static void emulate_cycle();
  static int tick_timers();
  
};

#endif // CHIP8_H