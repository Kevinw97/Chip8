#include "chip8.hpp"
#include <iostream>
#include <array>
#include <fstream>
#include <random>

std::array<unsigned char, 80> chip8_fontset =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8& Chip8::get()
{
  static Chip8 instance;
  return instance;
}

Chip8::Chip8()
{
  reset();
}

// START DECLARATIONS

std::array<unsigned char, 4096>     Chip8::memory;
std::array<unsigned char, 16>       Chip8::V;
std::array<unsigned short, 16>      Chip8::stack;

unsigned short                      Chip8::I;
unsigned short                      Chip8::pc;
unsigned short                      Chip8::sp;

unsigned char                       Chip8::delay_timer;
unsigned char                       Chip8::sound_timer;

std::array<unsigned char, 64 * 32>  Chip8::graphics;
std::array<unsigned char, 16>       Chip8::keys;

unsigned char                       Chip8::render_flag; // Currently not used since display constantly renders at 60 Hz

// END DECLARATIONS

void Chip8::reset()
{
  std::fill(memory.begin(), memory.end(), 0);
  std::fill(V.begin(), V.end(), 0);
  std::fill(stack.begin(), stack.end(), 0);
  std::fill(keys.begin(), keys.end(), 0);
  std::fill(graphics.begin(), graphics.end(), 0);

  I = 0;
  pc = 0x200;
  sp = 0;

  delay_timer = 0;
  sound_timer = 0;

  // Load fontset in memory
  std::copy(chip8_fontset.begin(), chip8_fontset.end(), memory.begin() + 0x50);

  return;
}

int Chip8::load(const char* file_path)
{
  reset();
  std::ifstream file(file_path, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();

  if (size > sizeof(memory) - 0x200)
  {
    std::cout << "ROM too big to fit in memory, or does not exist." << std::endl;
    return -1;
  }

  file.seekg(0, std::ios::beg);
  if (!file.read(reinterpret_cast<char*>(memory.data() + 0x200), size))
  {
    std::cout << "Failed to read file into memory!" << std::endl;
    return -1;
  }

  printf("Read ROM: %s with size %lld \n", file_path, size);

  return 0;
}

void Chip8::op_default(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  // Do nothing
}

void Chip8::op_0nnn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  pc = opcode & 0x0FFF;
}

void Chip8::op_00e0(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  std::fill(graphics.begin(), graphics.end(), 0);
  render_flag = 1;
}

void Chip8::op_00ee(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  sp--;
  pc = stack[sp];
}

void Chip8::op_1nnn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  pc = opcode & 0x0FFF;
}

void Chip8::op_2nnn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  stack[sp] = pc;
  sp++;
  pc = opcode & 0x0FFF;
}

void Chip8::op_3xkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  if (V[x] == val)
  {
    pc += 2;
  }
}

void Chip8::op_4xkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  if (V[x] != val)
  {
    pc += 2;
  }
}

void Chip8::op_5xy0(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  if (V[x] != val)
  {
    pc += 2;
  }
}

void Chip8::op_6xkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  V[x] = val;
}

void Chip8::op_7xkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  V[x] += val;
}

void Chip8::op_8xy0(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  V[x] = V[y];
}

void Chip8::op_8xy1(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  V[x] |= V[y];
}

void Chip8::op_8xy2(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  V[x] &= V[y];
}

void Chip8::op_8xy3(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  V[x] ^= V[y];
}

void Chip8::op_8xy4(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  V[x] += V[y];
  V[0xF] = (V[x] < V[y]) ? 1 : 0; // Set V[0xF] if overflow occurred
}

void Chip8::op_8xy5(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  bool carry = V[x] >= V[y];
  V[x] -= V[y];
  V[0xF] = carry;
}

void Chip8::op_8xy6(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  bool carry = V[x] & 0x0001;
  V[x] >>= 1;
  V[0xF] = carry;
}

void Chip8::op_8xy7(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  bool carry = V[y] >= V[x];
  V[x] = V[y] - V[x];
  V[0xF] = carry;
}

void Chip8::op_8xye(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  unsigned char carry = (V[x] & 0x80) >> 7;
  V[x] <<= 1;
  V[0xF] = carry;
}

void Chip8::op_9xy0(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  if (V[x] != V[y])
  {
    pc += 2;
  }
}

void Chip8::op_annn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  I = opcode & 0x0FFF;
}

void Chip8::op_bnnn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  pc = (opcode & 0x0FFF) + V[0];
}

void Chip8::op_cxkk(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  unsigned char random_byte = rand() % 0x100;
  V[x] = random_byte & val;
}

void Chip8::op_dxyn(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  unsigned char n = opcode & 0x000F;

  unsigned char x_coord = V[x];
  unsigned char y_coord = V[y];

  V[0xF] = 0;

  for (int y_pixel = 0; y_pixel < n; y_pixel++)
  {
    unsigned char sprite_row = memory[I + y_pixel];
    for (int x_pixel = 0; x_pixel < 8; x_pixel++)
    {
      if (sprite_row & (0x80 >> x_pixel)) // Pixel on sprite is set
      {
        if (graphics[((y_coord + y_pixel) % 32) * 64 + ((x_coord + x_pixel) % 64)]) // Pixel on display is set
        {
          V[0xF] = 1;
        }
        graphics[((y_coord + y_pixel) % 32) * 64 + ((x_coord + x_pixel) % 64)] ^= 0xFF;
      }
    }
  }

  render_flag = 1;
}

void Chip8::op_ex9e(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  if (keys[V[x]])
  {
    pc += 2;
  }
}

void Chip8::op_exa1(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  if (!keys[V[x]])
  {
    pc += 2;
  }
}

void Chip8::op_fx07(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  V[x] = delay_timer;
}

void Chip8::op_fx0a(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  static unsigned char key = 0xFF;
  if (key == 0xFF)
  {
    for (int i = 0; i < 16; ++i)
    {
      if (keys[i]) // Key is held down, save it, and wait until it is released
      {
        key = i;
      }
    }
    pc -= 2;
  }
  else
  {
    if (!keys[key]) // Key is released, we can stop waiting and save the key
    {
      V[x] = key;
      key = 0xFF;
    }
  }
}

void Chip8::op_fx15(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  delay_timer = V[x];
}

void Chip8::op_fx18(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  sound_timer = V[x];
}

void Chip8::op_fx1e(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  I += V[x];
}

void Chip8::op_fx29(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  I = 0x50 + (V[x] * 5);
}

void Chip8::op_fx33(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  memory[I] = (V[x] / 100);
  memory[I + 1] = (V[x] / 10) % 10;
  memory[I + 2] = V[x] % 10;
}

void Chip8::op_fx55(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  std::copy(V.begin(), V.begin() + x + 1, memory.begin() + I);
}

void Chip8::op_fx65(unsigned short opcode, unsigned char x, unsigned char y, unsigned char val)
{
  std::copy(memory.begin() + I, memory.begin() + I + x + 1, V.begin());
}

/*
First opcode is easy to switch on, it is simply any value from 0x0 to 0xF
This requires 2 keys for lookup, use first byte, and if it can further diverge, need second key
Function pointer will be stored in an array of size 16 with unordered_maps
*/

opcode_function Chip8::get_function(unsigned short opcode)
{
  //
  // START static function table declaration 
  //

  static std::array <std::unordered_map<short, opcode_function>, 16> op_table = {
    std::unordered_map<short, opcode_function> {                                                                  // 0x0
      { 0x00e0, op_00e0 },
      { 0x00ee, op_00ee },
      { 0x0000, op_0nnn }
    },
    std::unordered_map<short, opcode_function> {{ 0x0000, op_1nnn }},                                             // 0x1
    std::unordered_map<short, opcode_function> {{ 0x0000, op_2nnn }},                                             // 0x2
    std::unordered_map<short, opcode_function> {{ 0x0000, op_3xkk }},                                             // 0x3
    std::unordered_map<short, opcode_function> {{ 0x0000, op_4xkk }},                                             // 0x4
    std::unordered_map<short, opcode_function> {{ 0x0000, op_5xy0 }},                                             // 0x5
    std::unordered_map<short, opcode_function> {{ 0x0000, op_6xkk }},                                             // 0x6
    std::unordered_map<short, opcode_function> {{ 0x0000, op_7xkk }},                                             // 0x7
    std::unordered_map<short, opcode_function> {                                                                  // 0x8
      { 0x0000, op_8xy0 },
      { 0x0001, op_8xy1 },
      { 0x0002, op_8xy2 },
      { 0x0003, op_8xy3 },
      { 0x0004, op_8xy4 },
      { 0x0005, op_8xy5 },
      { 0x0006, op_8xy6 },
      { 0x0007, op_8xy7 },
      { 0x0008, op_8xye }
    },
    std::unordered_map<short, opcode_function> {{ 0x0000, op_9xy0 }},                                             // 0x9
    std::unordered_map<short, opcode_function> {{ 0x0000, op_annn }},                                             // 0xa
    std::unordered_map<short, opcode_function> {{ 0x0000, op_bnnn }},                                             // 0xb
    std::unordered_map<short, opcode_function> {{ 0x0000, op_cxkk }},                                             // 0xc
    std::unordered_map<short, opcode_function> {{ 0x0000, op_dxyn }},                                             // 0xd
    std::unordered_map<short, opcode_function> {                                                                  // 0xe
      { 0x009e, op_ex9e },
      { 0x00a1, op_exa1 }
    },                         
    std::unordered_map<short, opcode_function> {                                                                  // 0xf
      { 0x0007, op_fx07 },
      { 0x000a, op_fx0a },
      { 0x0015, op_fx15 },
      { 0x0018, op_fx18 },
      { 0x001e, op_fx1e },
      { 0x0029, op_fx29 },
      { 0x0033, op_fx33 },
      { 0x0055, op_fx55 },
      { 0x0065, op_fx65 }
    }
  };

  //
  // END static function table declaration 
  //

  unsigned short index = (opcode & 0xF000) >> 12;
  unsigned short map_key = 0x0000;
  if (index == 0x0008)
    map_key = opcode & 0x000F;
  if (index == 0x000e || index == 0x000f)
    map_key = opcode & 0x00FF;
  if (index == 0x0000)
    map_key = opcode & 0xFFF;

  auto func_iterator = op_table[index].find(map_key);

  if (func_iterator == op_table[index].end())
  {
    return Chip8::op_default;
  }

  return func_iterator->second;
}

void Chip8::emulate_cycle()
{
  unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
  unsigned char x = (0x0F00 & opcode) >> 8;
  unsigned char y = (0x00F0 & opcode) >> 4;
  unsigned char val = opcode & 0x00FF;
  pc += 2;
  get_function(opcode)(opcode, x, y, val);
  return;
}

/*
Updates the delay and sound timer, should be called at a 60 Hz rate
Returns error code indicating which timer is nonzero.
*/
int Chip8::tick_timers()
{
  int ret = CHIP8_ALL_TIMERS_ZERO;

  if (delay_timer > 0)
  {
    --delay_timer;
    if (delay_timer)
    {
      ret |= CHIP8_DELAY_TIMER_NONZERO;
    }
  }
  if (sound_timer > 0)
  {
    --sound_timer;
    if (sound_timer)
    {
      ret |= CHIP8_SOUND_TIMER_NONZERO;
    }
  }

  return ret;
}
