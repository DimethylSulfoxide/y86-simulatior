#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "y86.h"

int main(char argc, char **argv)
{
    if (argc == 1)
    {
        printf("Usage:\n\ty86.exe <filename>.\n");
        exit(-1);
    }
    char filename[100];
    strcpy(filename, argv[1]);

    init(filename);
    while (1)
    {
        exec_single_instr();
    }
    return 0;
}