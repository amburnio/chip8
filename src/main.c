#include <stdio.h>
#include "chip8.h"

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

Chip8 core;
Chip8 *core_ptr = &core;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Chip-8", NULL, NULL);

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize window and renderer
    if (!SDL_CreateWindowAndRenderer("Chip-8", 640, 320, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, 640, 320, SDL_LOGICAL_PRESENTATION_DISABLED);

    initialize(core_ptr);

    // Load game into memory
    const char *game_path = argv[1];
    FILE *rom = fopen(game_path, "rb");
    if (rom == NULL) {
        printf("Failed to open rom!");
        return SDL_APP_FAILURE;
    }
    unsigned char buffer[2];
    int mem_index = 512;
    while (fread(buffer, 1, 2, rom) != 0) {
        for (int i = 0; i < 2; ++i) {
            core.memory[mem_index] = buffer[i];
            ++mem_index;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_FRect rect;

    // Set background to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Draw white square
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    rect.x = rect.y = 0;
    rect.w = rect.h = 10;
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // Shutdown app
}