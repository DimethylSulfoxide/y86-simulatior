#include <inttypes.h>

#define AOK 1
#define HLT 2
#define ADR 3
#define INS 4

#define MEM_LENGTH 96
// 0xed
#define OOM 237
#define BYTES_PER_WORD 8 

typedef unsigned char uchar;

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