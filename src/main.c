#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "y86.h"

void print_usage();

int main(char argc, char **argv)
{

    char ch;
    char filename[100];
    char out_filename[100];
    int flag = 0;
    int enable_debug = 0;

    if (argc == 1)
    {
        print_usage();
        return -1;
    }

    while ((ch = getopt(argc, argv, "df:o:")) != -1)
    {
        switch (ch)
        {
        case 'd':
            enable_debug = 1;
            break;

        case 'f':
            strcpy(filename, optarg);
            flag = 1;
            break;
        case 'o':
            strcpy(out_filename, optarg);
            break;
        }
    }

    if (flag)
        ;
    else
    {
        printf("Lacking argument -f <filename>. Abort.\n");
        exit(-1);
    }

    init(filename);
    while (1)
    {
        if (enable_debug)
            exec_single_instr_debug();
        else
            exec_single_instr();
    }
    return 0;
}

void print_usage()
{
    printf("Usage:\n\t<executable filename> -f <filename> [-d] [-o <output filename>]\n\n");
    printf("\t-f <filename>\t\t: Hex code to execute. Use double backward slash on Windows.\n");
    printf("\t[-d]\t\t\t: Enable debug mode. \n");
    printf("\t[-o <output filename>]\t: Write the state of this emulator to file when program ends.\n");
}