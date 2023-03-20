#include <inttypes.h>

#define AOK 1
#define HLT 2
#define ADR 3
#define INS 4;

// extern int64_t RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI, R8, R9, R10, R11, R12, R13, R14, NIL;
extern int64_t REGS[15];

extern unsigned char ZF, SF, OF;
extern int8_t STAT;
extern int64_t MEM [1024];
extern int64_t PC;