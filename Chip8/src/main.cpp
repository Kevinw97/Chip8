#include <SDL3/SDL.h>
#include <iostream>

#include "chip8.h"

using namespace std;

unsigned char key_map[16] = {
  SDLK_X, // 0
  SDLK_1, // 1
  SDLK_2, // 2
  SDLK_3, // 3
  SDLK_Q, // 4
  SDLK_W, // 5
  SDLK_E, // 6
  SDLK_A, // 7
  SDLK_S, // 8
  SDLK_D, // 9
  SDLK_Z, // A
  SDLK_C, // B
  SDLK_4, // C
  SDLK_R, // D
  SDLK_F, // E
  SDLK_V  // F
};

int main(int argc, char** argv)
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

  SDL_Event sdl_event;

  chip8.load(argv[1]);

  while (true)
  {
    // Process key events
    while (SDL_PollEvent(&sdl_event))
    {
      if (sdl_event.type == SDL_EVENT_QUIT)
      {
        return 0;
      }
      if (sdl_event.type == SDL_EVENT_KEY_DOWN)
      {
        if (sdl_event.key.key == SDLK_F5) // Reset
        {
          chip8.load(argv[1]);
          break;
        }
        for (int i = 0; i < 16; i++)
        {
          if (sdl_event.key.key == key_map[i])
          {
            chip8.keys[i] = 1;
          }
        }

      }
      if (sdl_event.type == SDL_EVENT_KEY_UP)
      {
        for (int i = 0; i < 16; i++)
        {
          if (sdl_event.key.key == key_map[i])
          {
            chip8.keys[i] = 0;
            chip8.key_up_fx0a(i);
          }
        }
      }
    }

    // Run single CPU cycle
    chip8.emulate_cycle();

    SDL_Delay(1);

    // Re-render if needed
    if (true || chip8.render_flag)
    {
      chip8.render_flag = 0;

      SDL_UpdateTexture(sdl_texture, NULL, chip8.graphics, 64);
      SDL_RenderTexture(renderer, sdl_texture, NULL, NULL);
      SDL_RenderPresent(renderer);
    }
  }

  return 0;
}