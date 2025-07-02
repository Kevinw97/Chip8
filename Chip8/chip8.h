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

	unsigned char waiting_for_key; // 0xFF when not set, otherwise used to store which register to load key into

	void reset();

public:
	unsigned char graphics[64 * 32]; // Width * Height : 64 * 32
	unsigned char render_flag;

	unsigned char keys[16];

	Chip8();
	~Chip8();

	int load(const char* file_path);
	void emulate_cycle();
	void key_up_fx0a(unsigned char key);
	
};

#endif // CHIP8_H