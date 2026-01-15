#ifndef CHIP8_H
#define CHIP8_H

typedef struct {
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned char display[64][32];
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char key[16];
    unsigned char key_down;
    unsigned char draw_flag;
} Chip8;

void initialize(Chip8 *core);
void emulate_cycle(Chip8 *core);

#endif