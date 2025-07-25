#include "chip8.hpp"
#include <iostream>
#include <fstream>
#include <random>

unsigned char chip8_fontset[80] =
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

Chip8::Chip8()
{
  reset();
}
Chip8::~Chip8() {}

void Chip8::reset()
{
  memset(memory, 0, sizeof(memory));
  memset(V, 0, sizeof(V));
  memset(stack, 0, sizeof(stack));
  memset(keys, 0, sizeof(keys));
  memset(graphics, 0, sizeof(graphics));

  I = 0;
  pc = 0x200;
  sp = 0;

  delay_timer = 0;
  sound_timer = 0;

  // Load fontset in memory
  memcpy_s(memory + 0x50, sizeof(memory) - 0x50, chip8_fontset, sizeof(chip8_fontset));

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
  if (!file.read(reinterpret_cast<char*>(memory) + 0x200, size))
  {
    std::cout << "Failed to read file into memory!" << std::endl;
    return -1;
  }

  return 0;
}

void Chip8::emulate_cycle()
{
  unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
  unsigned char x = (0x0F00 & opcode) >> 8;
  unsigned char y = (0x00F0 & opcode) >> 4;
  unsigned char val = opcode & 0x00FF;
  pc += 2;
  // std::cout << std::hex << opcode << std::endl;
  switch (opcode & 0xF000)
  {
  case 0x0000:
  {
    switch (opcode & 0x0FFF)
    {
    case 0x00E0: // 00E0 - CLS
    {
      memset(graphics, 0, sizeof(graphics));
      render_flag = 1;
      break;
    }
    case 0x00EE: // 00EE - RET
    {
      sp--;
      pc = stack[sp];
      break;
    }
    default: // 0nnn - SYS addr (unimplemented)
    {
      // pc = opcode & 0x0FFF;
      break;
    }
    }
    break;
  }
  case 0x1000: // 1nnn - JP addr
  {
    pc = opcode & 0x0FFF;
    break;
  }
  case 0x2000: // 2nnn - CALL addr
  {
    stack[sp] = pc;
    sp++;
    pc = opcode & 0x0FFF;
    break;
  }
  case 0x3000: // 3xkk - SE Vx, byte
  {
    if (V[x] == val)
    {
      pc += 2;
    }
    break;
  }
  case 0x4000: // 4xkk - SNE Vx, byte
  {
    if (V[x] != val)
    {
      pc += 2;
    }
    break;
  }
  case 0x5000: // 5xy0 - SE Vx, Vy
  {
    if (V[x] == V[y])
    {
      pc += 2;
    }
    break;
  }
  case 0x6000: // 6xkk - LD Vx, byte
  {
    V[x] = val;
    break;
  }
  case 0x7000: // 7xkk - ADD Vx, byte
  {
    V[x] += val;
    break;
  }
  case 0x8000:
  {
    switch (opcode & 0x000F)
    {
    case 0x0000: // 8xy0 - LD Vx, Vy
    {
      V[x] = V[y];
      break;
    }
    case 0x0001: // 8xy1 - OR Vx, Vy
    {
      V[x] |= V[y];
      break;
    }
    case 0x0002: // 8xy2 - AND Vx, Vy
    {
      V[x] &= V[y];
      break;
    }
    case 0x0003: // 8xy3 - XOR Vx, Vy
    {
      V[x] ^= V[y];
      break;
    }
    case 0x0004: // 8xy4 - ADD Vx, Vy
    {
      V[x] += V[y];
      V[0xF] = (V[x] < V[y]) ? 1 : 0; // Set V[0xF] if overflow occurred
      break;
    }
    case 0x0005: // 8xy5 - SUB Vx, Vy
    {
      bool carry = V[x] >= V[y];
      V[x] -= V[y];
      V[0xF] = carry;
      break;
    }
    case 0x0006: // 8xy6 - SHR Vx {, Vy}
    {
      bool carry = V[x] & 0x0001;
      V[x] >>= 1;
      V[0xF] = carry;
      break;
    }
    case 0x0007: // 8xy7 - SUBN Vx, Vy
    {
      bool carry = V[y] >= V[x];
      V[x] = V[y] - V[x];
      V[0xF] = carry;
      break;
    }
    case 0x000E: // 8xyE - SHL Vx {, Vy}
    {
      unsigned char carry = (V[x] & 0x80) >> 7;
      V[x] <<= 1;
      V[0xF] = carry;
      break;
    }
    }
    break;
  }
  case 0x9000: // 9xy0 - SNE Vx, Vy
  {
    if (V[x] != V[y])
    {
      pc += 2;
    }
    break;
  }
  case 0xA000: // Annn - LD I, addr
  {
    I = opcode & 0x0FFF;
    break;
  }
  case 0xB000: // Bnnn - JP V0, addr
  {
    pc = (opcode & 0x0FFF) + V[0];
    break;
  }
  case 0xC000: // Cxkk - RND Vx, byte
  {
    unsigned char random_byte = rand() % 0x100;
    V[x] = random_byte & val;
    break;
  }
  case 0xD000: // Dxyn - DRW Vx, Vy, nibble
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

    break;
  }
  case 0xE000:
  {
    switch (opcode & 0x00FF)
    {
    case 0x009E: // Ex9E - SKP Vx
    {
      if (keys[V[x]])
      {
        pc += 2;
      }
      break;
    }
    case 0x00A1: // ExA1 - SKNP Vx
    {
      if (!keys[V[x]])
      {
        pc += 2;
      }
      break;
    }
    }
    break;
  }
  case 0xF000:
  {
    switch (opcode & 0x00FF)
    {
    case 0x0007: // Fx07 - LD Vx, DT
    {
      V[x] = delay_timer;
      break;
    }
    case 0x000A: // Fx0A - LD Vx, K
    {
      static bool key_pressed = false;
      static unsigned char key = 0xFF;
      if (!key_pressed)
      {
        for (int i = 0; i < 16; ++i)
        {
          if (keys[i]) // Key is held down, save it, and wait until it is released
          {
            key_pressed = true;
            key = i;
          }
        }
      }
      else
      {
        if (!keys[key]) // Key is released, we can stop waiting and save the key
        {
          V[x] = key;
          key_pressed = false;
          key = 0xFF;
          pc += 2;
        }
      }
      pc -= 2;
      break;
    }
    case 0x0015: // Fx15 - LD DT, Vx
    {
      delay_timer = V[x];
      break;
    }
    case 0x0018: // Fx18 - LD ST, Vx
    {
      sound_timer = V[x];
      break;
    }
    case 0x001E: // Fx1E - ADD I, Vx
    {
      I += V[x];
      break;
    }
    case 0x0029: // Fx29 - LD F, Vx
    {
      I = 0x50 + (V[x] * 5);
      break;
    }
    case 0x0033: // Fx33 - LD B, Vx
    {
      memory[I] = (V[x] / 100);
      memory[I + 1] = (V[x] / 10) % 10;
      memory[I + 2] = V[x] % 10;
      break;
    }
    case 0x0055: // Fx55 - LD [I], Vx
    {
      for (unsigned char i = 0; i <= x; ++i)
      {
        memory[I++] = V[i];
      }
      break;
    }
    case 0x0065: // Fx65 - LD Vx, [I]
    {
      for (unsigned char i = 0; i <= x; ++i)
      {
        V[i] = memory[I++];
      }
      break;
    }
    }
    break;
  }
  }
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
