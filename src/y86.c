#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "y86.h"

int init(void);

int main(char argc, char ** argv) {
    return 0;
}

int init(void) {
    memset(REGS, 0, sizeof(int64_t) * 15);
    ZF = SF = OF = 0;
    PC = 0;
    STAT = AOK;
    return 0;
}