#include "chip8.h"
#include <iostream>
#include <fstream>

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
	// Reset everything in the emulator
	this->reset();

	// Load fontset in memory
	memcpy_s(this->memory + 0x50, sizeof(this->memory) - 0x50, chip8_fontset, sizeof(chip8_fontset));
}
Chip8::~Chip8() {}

void Chip8::reset() 
{
	memset(memory, 0, sizeof(memory));
	memset(V, 0, sizeof(V));
	memset(stack, 0, sizeof(stack));

	I = 0;
	pc = 0x200;
	sp = 0;

	delay_timer = 0;
	sound_timer = 0;
	return;
}

int Chip8::load(const char* file_path)
{
	reset();
	std::ifstream file(file_path, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();

	if (size > sizeof(this->memory) - 0x200)
	{
		std::cout << "ROM too big to fit in memory!" << std::endl;
		return -1;
	}

	file.seekg(0, std::ios::beg);
	if (!file.read(reinterpret_cast<char*>(this->memory), size))
	{
		std::cout << "Failed to read file into memory!" << std::endl;
		return -1;
	}

	return 0;
}

void Chip8::emulate_cycle()
{
	unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
	switch (opcode & 0xF000)
	{
		case 0x1000: // 1nnn - JP addr
		{
			pc = opcode & 0x0FFF;
			break;
		}
		case 0x2000: // 2nnn - CALL addr
		{
			stack[sp] = pc;
			sp++;
			pc = 0x0FFF;
			break;
		}
		case 0x3000: // 3xkk - SE Vx, byte
		{
			unsigned char x = (0x0F00 & opcode) >> 8;
			unsigned char val = opcode & 0x00FF;
			if (V[x] == val)
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
			break;
		}
		case 0x4000: // 4xkk - SNE Vx, byte
		{
			unsigned char x = (0x0F00 & opcode) >> 8;
			unsigned char val = opcode & 0x00FF;
			if (V[x] != val)
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
			break;
		}
		case 0x5000: // 5xy0 - SE Vx, Vy
		{
			unsigned char x = (0x0F00 & opcode) >> 8;
			unsigned char y = (0x00F0 & opcode) >> 4;
			if (V[x] == V[y])
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
			break;
		}
		case 0x6000: // 6xkk - LD Vx, byte
		{
			unsigned char x = (0x0F00 & opcode) >> 8;
			unsigned char val = 0x00FF & opcode;
			V[x] = val;
			pc += 2;
			break;
		}
		case 0x7000: // 7xkk - ADD Vx, byte
		{
			unsigned char x = (0x0F00 & opcode) >> 8;
			unsigned char val = 0x00FF & opcode;
			V[x] += val;
		}
		case 0x8000:
		{
			switch (opcode & 0x000F)
			{
				unsigned char x = (0x0F00 & opcode) >> 8;
				unsigned char y = (0x00F0 & opcode) >> 4;
				case 0x0000: // 8xy0 - LD Vx, Vy
				{
					V[x] = V[y];
				}
				case 0x0001: // 8xy1 - OR Vx, Vy
				{
					V[x] = V[x] | V[y];
				}
				case 0x0002: // 8xy2 - AND Vx, Vy
				{
					V[x] = V[x] & V[y];
				}
				case 0x0003: // 8xy3 - XOR Vx, Vy
				{
					V[x] = V[x] ^ V[y];
				}
				case 0x0004: // 8xy4 - ADD Vx, Vy
				{
					V[x] += V[y];
					if (V[x] < V[y]) // Set V[0xF] if overflow occurred
					{
						V[0xF] = 1;
					}
					else
					{
						V[0xF] = 0;
					}
				}
				case 0x0005: // 8xy5 - SUB Vx, Vy
				{
					if (V[x] > V[y]) {
						V[0xF] = 1;
					}
					else
					{
						V[0xF] = 0;
					}
					V[x] -= V[y];
				}
				case 0x0006: // 8xy6 - SHR Vx {, Vy}
				{
					if (V[x] & 0x0001)
					{
						V[0xF] = 1;
					}
					else
					{
						V[0xF] = 0;
					}
					V[x] >>= 1;
				}
				case 0x0007: // 8xy7 - SUBN Vx, Vy
				{
					if (V[y] > V[x])
					{
						V[0xF] = 1;
					}
					else
					{
						V[0xF] = 0;
					}
					V[x] = V[y] - V[x];
				}
				case 0x000E: // 8xyE - SHL Vx {, Vy}
				{
					if (V[x] & 0x80)
					{
						V[0xF] = 1;
					}
					else
					{
						V[0xF] = 0;
					}
					V[x] <<= 1;
				}
			}
			pc += 2;
			break;
		}
		case 0x9000: // 9xy0 - SNE Vx, Vy
		{
			unsigned char x = (0x0F00 & opcode) >> 8;
			unsigned char y = (0x00F0 & opcode) >> 4;
			if (V[x] != V[y])
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
		}
	}
}