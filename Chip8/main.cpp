#include <SDL3/SDL.h>
#include <iostream>

#include "chip8.h"

using namespace std;

int main(int argc, char **argv)
{
	// Command usage, should take ROM file name
	if (argc != 2) {
		cout << "Usage: Chip8 <ROM file>" << endl;
		return 1;
	}

	// Initialize SDL window and renderer 
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	// 64 x 32 but is scaled by 10 because no devices are that low resolution nowadays
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer("Chip-8", 640, 320, 0, &window, &renderer);
	SDL_SetRenderScale(renderer, 10, 10);

	// Set render color to black, make canvas black
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// Set render color to white
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderPresent(renderer);

	Chip8 chip8 = Chip8();

	SDL_Texture* sdl_texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGB332,
		SDL_TEXTUREACCESS_STREAMING,
		64,
		32);
	SDL_SetTextureScaleMode(sdl_texture, SDL_SCALEMODE_NEAREST);

	chip8.load(argv[1]);

	while (true)
	{
		chip8.emulate_cycle();
		if (chip8.render_flag)
		{
			chip8.render_flag = 0;

			SDL_UpdateTexture(sdl_texture, NULL, chip8.graphics, 64);
			SDL_RenderTexture(renderer, sdl_texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}
	}

	SDL_Delay(10000);

	return 0;
}