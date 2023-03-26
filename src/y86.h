#include <inttypes.h>

#define AOK 1
#define HLT 2
#define ADR 3
#define INS 4

#define MEM_LENGTH 512
// 0xed
#define OOM 237
#define BYTES_PER_WORD 8 

typedef unsigned char uchar;

#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R8 8
#define R9 9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define NIL 15

int act_mem_len;

// int64_t RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI, R8, R9, R10, R11, R12, R13, R14, NIL;
int64_t REGS[16];

uchar ZF, SF, OF;
int8_t STAT;
uchar MEM [MEM_LENGTH];
int64_t PC;

char reasons[4][4] = {"AOK", "HLT", "ADR", "INS"};
char reg_names[16][4] = {
    "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "NIL"
};