#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "chip8.h"

int main(void) {
    Chip8 core;
    Chip8 *core_ptr = &core;
    const char *game_path = "/Users/zachamburn/Downloads/1-chip8-logo.ch8";

    // Initialize graphics and input and create window
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_INIT failed!");
        return -1;
    }
    SDL_Window *window = SDL_CreateWindow("Chip-8", 640, 320, SDL_WINDOW_METAL);
    SDL_Event event;

    // Initialize Chip-8 core
    initialize(core_ptr);

    // Load game into memory
    FILE *rom = fopen(game_path, "rb");
    if (rom == NULL) {
        return -1;
    }

    unsigned char buffer[2];
    int mem_index = 512;
    while (fread(buffer, 1, 2, rom) != 0) {
        for (int i = 0; i < 2; ++i) {
            core.memory[mem_index] = buffer[i];
            ++mem_index;
        }
    }

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