#include <stdio.h>
#include "chip8.h"

/** DATA **/

unsigned char chip8_font_set[80] = {
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

/** FUNCTIONS **/

void initialize(Chip8 *core) {
    core->pc = 0x200;
    core->opcode = 0;
    core->I = 0;
    core->sp = 0;
    core->delay_timer = 0;
    core->sound_timer = 0;

    // Clear display
    for (int i = 0; i < (64 * 32); ++i) {
        core->gfx[i] = 0;
    }

    // Clear stack, registers V0-VF, and key state
    for (int i = 0; i < 16; ++i) {
        core->stack[i] = 0;
        core->V[i] = 0;
        core->key[i] = 0;
    }

    // Clear memory
    for (int i = 0; i < 4096; ++i) {
        core->memory[i] = 0;
    }

    // Load font set
    for (int i = 0; i < 80; ++i) {
        core->memory[i] = chip8_font_set[i];
    }
}

void emulate_cycle(Chip8 *core) {
    // Fetch opcode
    core->opcode = core->memory[core->pc] << 8 | core->memory[core->pc + 1];

    // Decode opcode
    switch (core->opcode & 0xF000) {
        case 0x0000:
            switch (core->opcode & 0x000F) {
                case 0x0000: // 00E0: Clears the screen
                    for (int i = 0; i < (64 * 32); ++i) {
                        core->gfx[i] = 0;
                    }
                    core->pc += 2;
                break;

                case 0x000E: // 00EE: Returns from a subroutine
                    // Execute
                    core->pc += 2;
                break;

                default:
                    printf ("Unknown opcode [0x0000]: 0x%X\n", core->opcode);
                break;
            }
        break;

        case 0x6000: // 6XNN: Sets VX to NN
            core->V[(core->opcode & 0x0F00) >> 8] = core->opcode & 0x00FF;
            core->pc += 2;
        break;

        case 0xA000: // ANNN: Sets I to the address NNN
            core->I = core->opcode & 0x0FFF;
            core->pc += 2;
        break;

        case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
            // TODO
        break;

        default:
            printf("Unknown opcode: %X\n", core->opcode);
    }

    // Update timers
    if (core->delay_timer > 0)
        --core->delay_timer;

    if (core->sound_timer > 0) {
        if (core->sound_timer == 1)
            printf("BEEP!\n");
        --core->sound_timer;
    }
}