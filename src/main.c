#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "chip8.h"

chip8 core;

int main(void) {
    // Initialize graphics and input and create window
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_INIT failed!");
        return -1;
    }
    SDL_Window *window = SDL_CreateWindow("Chip-8", 640, 320, SDL_WINDOW_METAL);
    SDL_Event event;

    // Initialize Chip-8 core
    initialize(core);

    // Main loop
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }
    }

    SDL_Quit();

    return 0;
}