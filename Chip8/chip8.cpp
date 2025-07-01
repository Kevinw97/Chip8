#include "chip8.h"
#include <iostream>
#include <fstream>

Chip8::Chip8() {}
Chip8::~Chip8() {}

void Chip8::init() 
{
	memset(this->memory, 0, 4096);
	return;
}

void Chip8::load(const char* file_path)
{
	this->init();

	std::ifstream file(file_path, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	if (!file.read(reinterpret_cast<char*>(this->memory), size))
	{
		std::cout << "Failed to read file into memory!" << std::endl;
	}
	std::cout << "DEBUG" << std::endl;
}