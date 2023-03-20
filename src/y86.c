#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "y86.h"


int init(void);
uchar get_byte_from_mem(int64_t addr);
int64_t get_word_from_mem(int64_t addr);
int exec_single_instr(void);
int exception(int);
uchar get_ra(uchar rab);
uchar get_rb(uchar rab);

// int exec(uchar*);

int main(char argc, char ** argv) {
    init();
    uchar bytes[] = {0x30, 0xf0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x20, 0x02};
    memcpy(MEM, bytes, sizeof(uchar) * 12);
    int flag;
    while (1) {
        if ((flag = exec_single_instr())){
            printf("Simulation stopped due to %s.\n", reasons[flag]);
            exit(-1);
        }
    }
    return 0;
}

int init(void) {
    memset(REGS, 0, sizeof(int64_t) * 15);
    ZF = SF = OF = 0;
    PC = 0;
    STAT = AOK;
    return 0;
}

uchar get_byte_from_mem(int64_t addr) {
    if (addr >= MEM_LENGTH) {
        exception(ADR);
        return OOM;
    }
    else {
        PC ++;
        return MEM[addr];
    }
}

int64_t get_word_from_mem(int64_t addr) {
    int64_t a = 0;
    int64_t t = 0;
    // uchar t;
    for (int i = 0; i < 8; i ++) {
        t = get_byte_from_mem(PC);
        a = a | t << (i * 8);
        // a &= ((int64_t)get_byte_from_mem(PC)) << (i * 8);
    }
    return a;
}

int exception(int state) {
    STAT = state;
    return 0;
}

uchar get_ra(uchar rab) {
    return (rab & 0xf0) >> 4;
}

uchar get_rb(uchar rab) {
    return rab & 0x0f;
}

int exec_single_instr(void) {
    uchar op, rab, ra, rb;
    int64_t imm;

    op = get_byte_from_mem(PC);
    switch (op >> 4) {
        case 0x00: 
            // halt
            exception(HLT);
            return 1;
            break;
        
        case 0x01:
            // nop
            break;

        case 0x02:
            // rrmovq
            rab = get_byte_from_mem(PC);
            ra = get_ra(rab);
            rb = get_rb(rab);
            if (ra == 0x0f || rb == 0x0f) {
                exception(INS);
                break;
            }
            REGS[rb] = REGS[ra];
            break;

        case 0x03:
            // irmovq
            rab = get_byte_from_mem(PC);
            ra = get_ra(rab);
            rb = get_rb(rab);
            if (rb == 0x0f || ra != 0x0f) {
                exception(ADR);
                break;
            }
            imm = get_word_from_mem(PC);
            REGS[rb] = imm;
            break;

        default:
            break;
    }
    return 0;
}