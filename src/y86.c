#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "y86.h"


int init(void);
uchar read_byte_from_mem(int64_t addr);
int write_byte_to_mem(int64_t addr, uchar byte);
int64_t read_word_from_mem(int64_t addr);
int write_word_to_mem(int64_t addr, int64_t word);
int exec_single_instr(void);
int exception(int);
uchar get_ra(uchar rab);
uchar get_rb(uchar rab);

// int exec(uchar*);

int main(char argc, char ** argv) {
    init();
    uchar bytes[] = {
        0x30, 0xf0, 0x11, 0x22, 0x33, 0x44, 0x44, 0x33, 0x22, 0x11, 
        0x20, 0x01, 
        0x20, 0x02, 
        0x20, 0x03, 
        0x20, 0x04,
        0x60, 0x01,
        0x61, 0x02, 
        0x62, 0x03, 
        0x63, 0x04, 
        // 0x40, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // 0x50, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    };
    memcpy(MEM, bytes, sizeof(uchar) * 26);
    int flag;
    while (1) {
        if (exec_single_instr()){
            printf("Simulation stopped due to %s.\n", reasons[STAT -1]);
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

uchar read_byte_from_mem(int64_t addr) {
    if (addr >= MEM_LENGTH || addr < 0) {
        exception(ADR);
        return OOM;
    }
    else {
        // PC ++;
        return MEM[addr];
    }
}

int write_byte_to_mem(int64_t addr, uchar byte) {
    if (addr < 0 || addr >= MEM_LENGTH) {
        exception(ADR);
        return 1;
    }
    else {
        MEM[addr] = byte;
        return 0;
    }
}

int64_t read_word_from_mem(int64_t addr) {
    int64_t a = 0;
    int64_t t = 0;
    // uchar t;
    for (int i = 0; i < 8; i ++) {
        t = read_byte_from_mem(addr + i);
        a = a | t << (i * 8);
    }
    return a;
}

int write_word_to_mem(int64_t addr, int64_t word) {
    for (int i = 0; i < 8; i ++) {
        write_byte_to_mem(addr + i, 0xff & (word >> i * 8));
    }
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
    int64_t imm, tmp;

    op = read_byte_from_mem(PC);
    PC ++;
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
            rab = read_byte_from_mem(PC);
            PC ++;
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
            rab = read_byte_from_mem(PC);
            PC ++;
            ra = get_ra(rab);
            rb = get_rb(rab);
            if (rb == 0x0f || ra != 0x0f) {
                exception(ADR);
                break;
            }
            imm = read_word_from_mem(PC);
            PC += BYTES_PER_WORD;
            REGS[rb] = imm;
            break;

        case 0x04:
            // rmmovq
            rab = read_byte_from_mem(PC);
            PC ++;
            ra = get_ra(rab);
            rb = get_rb(rab);
            if (rb == 0x0f || ra == 0x0f) {
                exception(ADR);
                break;
            }
            imm = read_word_from_mem(PC);
            PC += BYTES_PER_WORD;

            write_word_to_mem(REGS[rb] + imm, REGS[ra]);
            break;

        case 0x05:
            // mrmovq
            rab = read_byte_from_mem(PC);
            PC ++;
            ra = get_ra(rab);
            rb = get_rb(rab);
            if (rb == 0x0f || ra == 0x0f) {
                exception(ADR);
                break;
            }
            imm = read_word_from_mem(PC);
            PC += BYTES_PER_WORD;

            REGS[rb] = read_word_from_mem(REGS[ra] + imm);
            break;

        case 0x06:
            // opQ
            rab = read_byte_from_mem(PC);
            PC ++;
            ra = get_ra(rab);
            rb = get_rb(rab);
            if (rb == 0x0f || ra == 0x0f) {
                exception(ADR);
                break;
            }
            switch (op & 0xf) {
                case 0:
                    tmp = REGS[ra] + REGS[rb];
                    // 两个加数符号位相同并且结果符号位与两者不同，则溢出
                    if ((!((0x1 & REGS[ra] >> 31)^(0x1 & REGS[rb] >> 31))) &&
                        ((0x1 & REGS[ra] >> 31)^(0x1 & tmp >> 31)))
                        OF = 0x1;
                    else OF = 0x0;

                    if (!tmp)   ZF = 0x1; else ZF = 0x0;
                    SF = 0x1 & tmp >> 31;


                    REGS[rb] = tmp;
                    break;
                
                case 1:
                    // 同加法, 第二个加数取反加一按加法处理
                    // REGS[rb] = ~REGS[rb] + 1;
                    tmp = REGS[ra] - REGS[rb];

                    if ((REGS[rb] == ~REGS[rb] + 1) && REGS[ra] > 0)
                        OF = 0x1;
                    
                    else {
                        REGS[rb] = ~REGS[rb] + 1;
                        if ((!((0x1 & REGS[ra] >> 31)^(0x1 & REGS[rb] >> 31))) &&
                            ((0x1 & REGS[ra] >> 31)^(0x1 & tmp >> 31)))
                            OF = 0x1;
                        else OF = 0x0;
                    }

                    if (!tmp)   ZF = 0x1; else ZF = 0x0;
                    SF = 0x1 & tmp >> 31;

                    REGS[rb] = tmp;
                    break;

                case 2:
                    // andq
                    REGS[rb] &= REGS[ra];

                    OF = 0;
                    if (!REGS[rb])  ZF = 0x1; else ZF = 0;
                    SF = 0x1 & tmp >> 31;
                    break;
                
                case 3:
                    // xorq
                    REGS[rb] ^= REGS[ra];

                    OF = 0;
                    if (!REGS[rb])  ZF = 0x1; else ZF = 0;
                    SF = 0x1 & tmp >> 31;
                    break;

                default:
                    exception(INS);
                    break;
            }


        default:
            break;
    }
    return STAT != 1;
}