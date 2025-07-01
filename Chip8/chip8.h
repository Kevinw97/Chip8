#ifndef CHIP8_H
#define CHIP8_H

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

	void init();

public:
	unsigned char graphics[64 * 32];

	Chip8();
	~Chip8();

	void emulate_cycle();
	void load(const char* file_path);
};

#endif // CHIP8_H