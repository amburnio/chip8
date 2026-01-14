#include <stdio.h>
#include <stdlib.h>
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
    for (int i = 0; i < 64; ++i) {
        for (int j = 0; j < 32; ++j) {
            core->display[i][j] = 0;
        }
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

    // Get values
    const int x = (core->opcode & 0x0F00) >> 8;
    const int y = (core->opcode & 0x00F0) >> 4;
    const int n = core->opcode & 0x000F;
    const int nn = core->opcode & 0x00FF;
    const int nnn = core->opcode & 0x0FFF;
    int x_coord;
    int y_coord;
    int tmp;

    // Decode opcode
    switch (core->opcode & 0xF000) {
        case 0x0000:
            switch (core->opcode & 0x000F) {
                case 0x0000: // 00E0: Clears the screen
                    for (int i = 0; i < 64; ++i) {
                        for (int j = 0; j < 32; ++j) {
                            core->display[i][j] = 0;
                        }
                    }
                    core->pc += 2;
                break;

                case 0x000E: // 00EE: Returns from a subroutine
                    --core->sp;
                    core->pc = core->stack[core->sp] + 2;
                    core->stack[core->sp] = 0;
                break;

                default:
                    printf ("Unknown opcode [0x0000]: 0x%X\n", core->opcode);
                break;
            }
        break;

        case 0x1000: // 1NNN: Jumps to address NNN
            core->pc = nnn;
        break;

        case 0x2000: // 2NNN: Calls subroutine at NNN
            core->stack[core->sp] = core->pc;
            ++core->sp;
            core->pc = nnn;
        break;

        case 0x3000: // 3XNN: Skips the next instruction if VX equals NN
            if (core->V[x] == nn) core->pc += 2;
            core->pc += 2;
        break;

        case 0x4000: // 4XNN: Skips the next instruction if VX does not equal NN
            if (core->V[x] != nn) core->pc += 2;
            core->pc += 2;
        break;

        case 0x5000: // 5XY0: Skips the next instruction if VX equals VY
            if (core->V[x] == core->V[y]) core->pc += 2;
            core->pc += 2;
        break;

        case 0x6000: // 6XNN: Sets VX to NN
            core->V[x] = nn;
            core->pc += 2;
        break;

        case 0x7000: // 7XNN: Adds NN to VX (carry flag is not changed)
            core->V[x] += nn;
            core->pc += 2;
        break;

        case 0x8000:
            switch (core->opcode & 0x000F) {
                case 0x0000: // 8XY0: Sets VX to the value of VY
                    core->V[x] = core->V[y];
                    core->pc += 2;
                break;

                case 0x0001: // 8XY1: Sets VX to VX or VY
                    core->V[x] |= core->V[y];
                    core->pc += 2;
                break;

                case 0x0002: // 8XY2: Sets VX to VX and VY
                    core->V[x] &= core->V[y];
                    core->pc += 2;
                break;

                case 0x0003: // 8XY3: Sets VX to VX xor VY
                    core->V[x] ^= core->V[y];
                    core->pc += 2;
                break;

                case 0x0004: // 8XY4: Adds VY to VX
                    tmp = core->V[x] + core->V[y];
                    core->V[x] += core->V[y];
                    if (tmp > 255) core->V[0xF] = 1;
                    else core->V[0xF] = 0;
                    core->pc += 2;
                break;

                case 0x0005: // 8XY5: VY is subtracted from VX
                    tmp = core->V[x];
                    core->V[x] -= core->V[y];
                    if (tmp >= core->V[y]) core->V[0xF] = 1;
                    else core->V[0xF] = 0;
                    core->pc += 2;
                break;

                case 0x0006: // 8XY6: Shifts VX to the right by 1
                    // TODO: Make configurable ambiguous instruction
                    tmp = core->V[x] & 0x01;
                    core->V[x] = core->V[x] >> 1;
                    core->V[0xF] = tmp;
                    core->pc += 2;
                break;

                case 0x0007: // 8XY7: Sets VX to VY minus VX
                    tmp = core->V[x];
                    core->V[x] = core->V[y] - core->V[x];
                    if (core->V[y] >= tmp) core->V[0xF] = 1;
                    else core->V[0xF] = 0;
                    core->pc += 2;
                break;

                case 0x000E: // 8XYE: Shifts VX to the left by 1
                    // TODO: Make configurable ambiguous instruction
                    tmp = (core->V[x] & 0x80) >> 7;
                    core->V[x] = core->V[x] << 1;
                    core->V[0xF] = tmp;
                    core->pc += 2;
                break;

                default:
                    printf ("Unknown opcode [0x8000]: 0x%X\n", core->opcode);
                break;
            }
        break;

        case 0x9000: // 9XY0: Skips the next instruction if VX does not equal VY
            if (core->V[x] != core->V[y]) core->pc += 2;
            core->pc += 2;
        break;

        case 0xA000: // ANNN: Sets I to the address NNN
            core->I = nnn;
            core->pc += 2;
        break;

        case 0xB000: // BNNN: Jumps to the address NNN plus V0
            // TODO: Make configurable ambiguous instruction
            core->pc = core->V[0x0] + nnn;
        break;

        case 0xC000: // CXNN: Sets VX to the result of a bitwise and operation on a random number and NN
            core->V[x] = rand() & nn;
            core->pc += 2;
        break;

        case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
            core->V[0xF] = 0;
            unsigned char byte;
            y_coord = core->V[y] % 32;
            for (int i = 0; i < n; ++i) {
                x_coord = core->V[x] % 64;
                if (y_coord >= 32) break;
                byte = core->memory[core->I + i];
                unsigned char mask = 0x80;
                for (int j = 0; j < 8; ++j) {
                    if (x_coord >= 64) break;
                    if (byte & mask) {
                        if (core->display[x_coord][y_coord] == 1) {
                            core->display[x_coord][y_coord] = 0;
                            core->V[0xF] = 1;
                        }
                        else {
                            core->display[x_coord][y_coord] = 1;
                        }
                    }
                    mask >>= 1;
                    ++x_coord;
                }
                ++y_coord;
            }
            core->pc += 2;
        break;

        case 0xF000:
            switch (core->opcode & 0x00FF) {
                case 0x001E: // FX1E: Adds VX to I, VF is not affected
                    // TODO: Add Amiga behavior
                    core->I += core->V[x];
                    core->pc += 2;
                break;

                case 0x0033: // FX33: Stores the binary-coded decimal representation of VX
                    tmp = core->V[x];
                    for (int i = 2; i >= 0; --i) {
                        core->memory[core->I + i] = tmp % 10;
                        tmp = tmp / 10;
                    }
                    core->pc += 2;
                break;

                case 0x0055: // FX55: Stores from V0 to VX (including VX) in memory, starting at address I
                    // TODO: Make configurable ambiguous instruction
                    for (int i = 0; i <= x; ++i) {
                        core->memory[core->I + i] = core->V[i];
                    }
                    core->pc += 2;
                break;

                case 0x0065: // FX65: Fills from V0 to VX (including VX) with values from memory, starting at address I
                    // TODO: Make configurable ambiguous instruction
                    for (int i = 0; i <= x; ++i) {
                        core->V[i] = core->memory[core->I + i];
                    }
                    core->pc += 2;
                break;

                default:
                    printf ("Unknown opcode [0xF000]: 0x%X\n", core->opcode);
                break;
            }
        break;

        default:
            printf("Unknown opcode: %X\n", core->opcode);
        break;
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