#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

Chip8 core;
Chip8 *core_ptr = &core;

// Debug
unsigned int last_time = 0, current_time;
int frames = 0;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Chip-8", NULL, NULL);
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60"); // FPS

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize window and renderer
    if (!SDL_CreateWindowAndRenderer("Chip-8", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_DISABLED);

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

    // Seed random number generator
    srand(time(NULL));

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
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Update timers
    if (core.delay_timer > 0)
        --core.delay_timer;

    if (core.sound_timer > 0) {
        if (core.sound_timer == 1)
            printf("BEEP!\n");
        --core.sound_timer;
    }

    // Get key states
    const bool *key_states = SDL_GetKeyboardState(NULL);
    core.key[0] = key_states[SDL_SCANCODE_X];
    core.key[1] = key_states[SDL_SCANCODE_1];
    core.key[2] = key_states[SDL_SCANCODE_2];
    core.key[3] = key_states[SDL_SCANCODE_3];
    core.key[4] = key_states[SDL_SCANCODE_Q];
    core.key[5] = key_states[SDL_SCANCODE_W];
    core.key[6] = key_states[SDL_SCANCODE_E];
    core.key[7] = key_states[SDL_SCANCODE_A];
    core.key[8] = key_states[SDL_SCANCODE_S];
    core.key[9] = key_states[SDL_SCANCODE_D];
    core.key[0xA] = key_states[SDL_SCANCODE_Z];
    core.key[0xB] = key_states[SDL_SCANCODE_C];
    core.key[0xC] = key_states[SDL_SCANCODE_4];
    core.key[0xD] = key_states[SDL_SCANCODE_R];
    core.key[0xE] = key_states[SDL_SCANCODE_F];
    core.key[0xF] = key_states[SDL_SCANCODE_V];

    // Emulate 11 cycles per frame (~660 ips)
    for (int i = 0; i < 11; ++i) {
        emulate_cycle(core_ptr);
    }

    // Draw pixels
    SDL_FRect pixel;
    pixel.w = pixel.h = 10;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < 64; ++i) {
        for (int j = 0; j < 32; ++j) {
            if (core.display[i][j] == 1) {
                pixel.x = (float) i * 10;
                pixel.y = (float) j * 10;
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    // Render frame
    SDL_RenderPresent(renderer);

    // Debug (Print FPS)
    current_time = SDL_GetTicks();
    if (current_time > last_time + 1000) {
        printf("Frames: %d\n", frames);
        last_time = current_time;
        frames = 0;
    }
    else {
        ++frames;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // Shutdown app
}