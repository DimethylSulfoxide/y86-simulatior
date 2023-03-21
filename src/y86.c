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
void print_regs();
void end();

// int exec(uchar*);

int main(char argc, char ** argv) {
    init();
    uchar bytes[] = {
        // 0x70, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        // 0x00,
        // 0x31
        0x30, 0xf0, 0x11, 0x22, 0x33, 0x44, 0x44, 0x33, 0x22, 0x11, 
        0x30, 0xf1, 0x11, 0x22, 0x33, 0x44, 0x44, 0x33, 0x22, 0x11,
        0x61, 0x01,
        0x23, 0x02
        // 0x20, 0x01, 
        // 0x20, 0x02, 
        // 0x20, 0x03, 
        // 0x20, 0x04,
        // 0x60, 0x01,
        // 0x61, 0x02, 
        // 0x62, 0x03, 
        // 0x63, 0x04, 
        // 0x40, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // 0x50, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    };
    memcpy(MEM, bytes, sizeof(uchar) * 24);
    int flag;
    while (1) {
        exec_single_instr();
    }
    return 0;
}

int init(void) {
    memset(REGS, 0, sizeof(int64_t) * 15);
    REGS[15] = 0xadeaddecadefaded;
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
    printf("Program end because of state %s.\n", reasons[STAT-1]);
    end();
    return 0;
}

void print_regs() {
    for (int i = 0; i < 8; i ++) {
        printf("%s\t\t%0.16" PRIX64 "\t\t%s\t\t%0.16" PRIX64 "\n", reg_names[i], REGS[i], reg_names[i + 8], REGS[i+8] );
    }
    printf("\n\n");
    printf("ZF\t\t%0.16X\t\tSTAT\t\t%0.1X\t%s\nSF\t\t%0.16X\t\tPC\t\t%0.16" PRIX64 "\nOF\t\t%0.16x\n", 
            ZF, STAT, reasons[STAT-1], SF, PC, OF);
}

void end() {
    print_regs();
    exit(-1);
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
            if (op & 0x0f) exception(INS);
            exception(HLT);
            return 1;
            break;
        
        case 0x01:
            // nop
            if (op & 0x0f) exception(INS);
            break;

        // case 0x02:
        //     // rrmovq
        //     rab = read_byte_from_mem(PC);
        //     PC ++;
        //     ra = get_ra(rab);
        //     rb = get_rb(rab);
        //     if (ra == 0x0f || rb == 0x0f) {
        //         exception(INS);
        //         break;
        //     }
        //     REGS[rb] = REGS[ra];
        //     break;

        case 0x02: 
            // cmovle, etc.
            rab = read_byte_from_mem(PC);
            PC ++;
            ra = get_ra(rab);
            rb = get_rb(rab);
            if (rb == 0x0f || ra == 0x0f) {
                exception(ADR);
                break;
            }
            switch (op & 0x0f) {
                case 0x00:
                    // rrmovq
                    REGS[rb] = REGS[ra];
                    break;
                
                case 0x01:
                    // cmovle
                    if (SF ^ OF | ZF)
                        REGS[rb] = REGS[ra];
                    break;
                
                case 0x02:
                    // cmovl
                    if (SF ^ OF)
                        REGS[rb] = REGS[ra];
                    break;
                
                case 0x03:
                    // cmove
                    if (ZF)
                        REGS[rb] = REGS[ra];
                    break;
                
                case 0x04:
                    // cmovne
                    if (~ZF)
                        REGS[rb] = REGS[ra];
                    break;
                
                case 0x05:
                    // cmovge
                    if (~(SF ^ OF))
                        REGS[rb] = REGS[ra];
                    break;

                case 0x06:
                    // cmovg
                    if (~(SF ^ OF) & ~ZF)
                        REGS[rb] = REGS[ra];
                    break;

                default:
                    // others 
                    exception(INS);
                    break;
            }
            break;

        case 0x03:
            // irmovq
            if (op & 0x0f) exception(INS);
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
            if (op & 0x0f) exception(INS);
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
            if (op & 0x0f) exception(INS);
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
            break;

        case 0x07: 
            imm = read_word_from_mem(PC);
            PC += BYTES_PER_WORD;
            if (imm < 0 || imm >= MEM_LENGTH) 
                exception(ADR);
            switch (op & 0x0f) {
                case 0x00:
                    // jmp
                    PC = imm;
                    break;
                
                case 0x01:
                    // jle
                    if (SF ^ OF | ZF)
                        PC = imm;
                    break;
                
                case 0x02:
                    // jl
                    if (SF ^ OF)
                        PC = imm;
                    break;
                
                case 0x03:
                    // je
                    if (ZF)
                        PC = imm;
                    break;
                
                case 0x04:
                    // jne
                    if (~ZF)
                        PC = imm;
                    break;
                
                case 0x05:
                    // jge
                    if (~(SF ^ OF))
                        PC = imm;
                    break;

                case 0x06:
                    // jg
                    if (~(SF ^ OF) & ~ZF)
                        PC = imm;
                    break;

                default:
                    // others 
                    exception(INS);
                    break;
            }
            break;

        



        default:
            exception(INS);
            break;
    }
    return STAT != 1;
}