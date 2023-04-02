#include "y86.h"

int64_t REGS[16];

uchar ZF, SF, OF;
int8_t STAT;
uchar MEM[MEM_LENGTH];
int64_t PC;
uchar bytes[MEM_LENGTH];
char DISASM_STRING[MAX_LENGTH_SINGLE_INSTR];

enum status
{
    AOK = 1,
    HLT,
    ADR,
    INS
};
enum regs
{
    RAX = 0,
    RCX,
    RDX,
    RBX,
    RSP,
    RBP,
    RSI,
    RDI,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    NIL
};

char status_string[5][4] = {"UND", "AOK", "HLT", "ADR", "INS"};
char reg_string[16][4] = {
    "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "NIL"};
char instr_string[][8] = {
    "halt",
    "nop",
    "rrmovq",
    "cmovle",
    "cmovl",
    "cmove",
    "cmovne",
    "cmovge",
    "cmovg",
    "irmovq",
    "rmmovq",
    "mrmovq",
    "addq",
    "subq",
    "andq",
    "xorq",
    "jmp",
    "jle",
    "jl",
    "je",
    "jne",
    "jge",
    "jg",
    "call",
    "ret",
    "pushq",
    "popq",
};

instr_func_p instr_func_list[] = {
    OPC00_halt,
    OPC01_nop,
    OPC02_cmovxx,
    OPC03_irmovq,
    OPC04_rmmovq,
    OPC05_mrmovq,
    OPC06_opq,
    OPC07_jxx,
    OPC08_call,
    OPC09_ret,
    OPC0A_pushq,
    OPC0B_popq};

int init(char *filename)
{
    memset(REGS, 0, sizeof(int64_t) * 15);
    REGS[15] = 0xadeaddecadefaded;
    ZF = SF = OF = 0;
    PC = 0;
    STAT = AOK;
    write_to_mem(filename);
    return 0;
}

// 从mem中读取一个字节; 不改变addr的值
uchar read_byte_from_mem(int64_t addr)
{
    if (addr >= MEM_LENGTH || addr < 0)
    {
        exception(ADR);
    }
    else
    {
        return MEM[addr];
    }
}

// 写入一个字节
int write_byte_to_mem(int64_t addr, uchar byte)
{
    if (addr < 0 || addr >= MEM_LENGTH)
    {
        exception(ADR);
        return 1;
    }
    else
    {
        MEM[addr] = byte;
        return 0;
    }
}

// 读取一个字,在这里是64位int
int64_t read_word_from_mem(int64_t addr)
{
    int64_t a = 0;
    int64_t t = 0;
    // uchar t;
    for (int i = 0; i < 8; i++)
    {
        t = read_byte_from_mem(addr + i);
        a = a | t << (i * 8);
    }
    return a;
}

// 写入一个字
int write_word_to_mem(int64_t addr, int64_t word)
{
    for (int i = 0; i < 8; i++)
    {
        write_byte_to_mem(addr + i, 0xff & (word >> i * 8));
    }
}

// 引发异常,打印系统状态,退出程序
int exception(int state)
{
    STAT = state;
    printf("Program end because of state %s.\n", status_string[STAT]);
    end();
    return 0;
}

// 打印寄存器
void print_regs()
{
    for (int i = 0; i < 8; i++)
    {
        printf("%s\t\t0x%0.16" PRIX64 "\t\t%s\t\t0x%0.16" PRIX64 "\n", reg_string[i], REGS[i], reg_string[i + 8], REGS[i + 8]);
    }
    printf("\n\n");
    printf("ZF\t\t0x%0.16X\t\tSTAT\t\t%0.1X\t%s\nSF\t\t0x%0.16X\t\tPC\t\t0x%0.16" PRIX64 "\nOF\t\t0x%0.16x\n",
           ZF, STAT, status_string[STAT], SF, PC, OF);
}

// 打印内存
void print_mem(int n)
{
    for (int i = 0; i < n; i += 8)
    {
        printf("%0.4X\t%0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X\n",
               i, MEM[i + 0], MEM[i + 1], MEM[i + 2], MEM[i + 3], MEM[i + 4], MEM[i + 5], MEM[i + 6], MEM[i + 7]);
    }
}

// 终止程序; hlt终止和aok终止()认为是正常推出
void end()
{
    print_regs();
    printf("\n\n\n\n");
    print_mem(MEM_LENGTH);
    if (STAT == AOK || STAT == HLT)
        exit(0);
    else
        exit(-1);
}

// 获取寄存器字节的左寄存器(源)
uchar get_ra(uchar rab)
{
    return (rab & 0xf0) >> 4;
}

// 获取目的寄存器
uchar get_rb(uchar rab)
{
    return rab & 0x0f;
}

int read_from_file(char *filename, uchar *dest)
{
    FILE *fp = fopen(filename, "rt");
    int i = 0;
    char ch;
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n' || ch == ' ' || ch == '\t')
            continue;

        else
        {
            fseek(fp, -1, SEEK_CUR);
            fscanf(fp, "%2x", dest + i);
            i++;
        }
    }
    return i;
}

int write_to_mem(char *filename)
{
    int i = read_from_file(filename, bytes);
    memcpy(MEM, bytes, sizeof(uchar) * i);
}

int exec_single_instr(void)
{
    uchar op, func, ra, rb, *op_p = &op, *func_p = &func, *ra_p = &ra, *rb_p = &rb;
    int64_t imm, *imm_p = &imm;
    int length = get_arguments(PC, op_p, func_p, ra_p, rb_p, imm_p);
    PC += length;
    instr_func_list[op](func_p, ra_p, rb_p, imm_p);
    return 0;
}

int exec_single_instr_debug(void)
{
    system("cls");
    uchar op, func, ra, rb, *op_p = &op, *func_p = &func, *ra_p = &ra, *rb_p = &rb;
    int64_t imm, *imm_p = &imm;
    int length = get_arguments(PC, op_p, func_p, ra_p, rb_p, imm_p);
    print_regs();
    disasm(op_p, func_p, ra_p, rb_p, imm_p);
    printf("%0.16" PRIX64 "\t%s\n", PC,  DISASM_STRING);
    system("pause");
    PC += length;
    instr_func_list[op](func_p, ra_p, rb_p, imm_p);
    return 0;
}

int disasm(uchar *op_p, uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    memset(DISASM_STRING, 0, MAX_LENGTH_SINGLE_INSTR);
    char tmp[40] = {0};
    int instr_name_index = 0;
    switch (*op_p)
    {
    case 0x0:
    case 0x1:
        instr_name_index = 0 + *op_p + *func_p;
        break;
    case 0x2:
        instr_name_index = 0 + *op_p + *func_p;
        break;
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
        instr_name_index = 0x6 + *op_p + *func_p;
        break;
    case 0x7:
        instr_name_index = 16 + *func_p;
        break;
    case 0x8:
    case 0x9:
    case 0xa:
    case 0xb:
        instr_name_index = 15 + *op_p;
        break;
    }
    strcat(DISASM_STRING, instr_string[instr_name_index]);
    strcat(DISASM_STRING, "\t");

    switch (*op_p)
    {
    case 0x0:
    case 0x1:
    case 0x9:
        break;
    case 0x7:
    case 0x8:
        sprintf(tmp, "0x%0.16" PRIX64, *imm_p);
        strcat(DISASM_STRING, tmp);
        break;
    
    case 0x2:
    case 0x6:
        sprintf(tmp, "%%%s, %%%s", reg_string[*ra_p], reg_string[*rb_p]);
        strcat(DISASM_STRING, tmp);
        break;

    case 0xa:
    case 0xb:
        sprintf(tmp, "%%%s", reg_string[*ra_p]);
        strcat(DISASM_STRING, tmp);
        break;

    case 0x3:
        sprintf(tmp, "0x%0.16" PRIX64 ", %%%s", *imm_p, reg_string[*rb_p]);
        strcat(DISASM_STRING, tmp);
        break;

    case 0x4:
        sprintf(tmp, "%%%s, %" PRId64 "(%%%s)", reg_string[*ra_p], *imm_p, reg_string[*rb_p]);
        strcat(DISASM_STRING, tmp);
        break;

    case 0x5:
        sprintf(tmp, "%" PRId64 "(%%%s), %%%s", *imm_p, reg_string[*rb_p], reg_string[*ra_p]);
        strcat(DISASM_STRING, tmp);
        break;
    }
}

int get_arguments(int64_t PC, uchar *op_p, uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    int length = 1;
    uchar opfu = read_byte_from_mem(PC);
    uchar rab;
    *op_p = opfu >> 4 & 0x0f;
    *func_p = opfu & 0xf;
    switch (*op_p)
    {
    case 0x0: // halt
    case 0x1: // nop
    case 0x9: // ret
        break;

    case 0x2: // cmovxx
    case 0x6: // opq
    case 0xa: // pushq
    case 0xb: // popq
        length += 1;
        rab = read_byte_from_mem(PC + 1);
        *ra_p = get_ra(rab);
        *rb_p = get_rb(rab);
        break;

    case 0x7: // jxx
    case 0x8: // call
        length += 8;
        *imm_p = read_word_from_mem(PC + 1);
        break;

    case 0x3: // irmovq
    case 0x4: // rmmovq
    case 0x5: // mrmovq
        length += 9;
        rab = read_byte_from_mem(PC + 1);
        *ra_p = get_ra(rab);
        *rb_p = get_rb(rab);
        *imm_p = read_word_from_mem(PC + 2);
        break;

    default:
        exception(INS);
        break;
    }

    return length;
}

void OPC00_halt(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*func_p)
    {
        exception(INS);
    }
    else
        exception(HLT);
}

void OPC01_nop(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*func_p)
        exception(INS);
    return;
}

void OPC02_cmovxx(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*rb_p == 0x0f || *ra_p == 0x0f)
    {
        exception(ADR);
    }
    switch (*func_p)
    {
    case 0x00:
        // rrmovq
        REGS[*rb_p] = REGS[*ra_p];
        break;

    case 0x01:
        // cmovle
        if ((SF ^ OF | ZF) & 0x1)
            REGS[*rb_p] = REGS[*ra_p];
        break;

    case 0x02:
        // cmovl
        if ((SF ^ OF) & 0x1)
            REGS[*rb_p] = REGS[*ra_p];
        break;

    case 0x03:
        // cmove
        if (ZF & 0x1)
            REGS[*rb_p] = REGS[*ra_p];
        break;

    case 0x04:
        // cmovne
        if ((~ZF) & 0x1)
            REGS[*rb_p] = REGS[*ra_p];
        break;

    case 0x05:
        // cmovge
        if ((~(SF ^ OF)) & 0x1)
            REGS[*rb_p] = REGS[*ra_p];
        break;

    case 0x06:
        // cmovg
        if ((~(SF ^ OF) & ~ZF) & 0x1)
            REGS[*rb_p] = REGS[*ra_p];
        break;

    default:
        // others
        exception(INS);
        break;
    }
}

void OPC03_irmovq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*func_p)
        exception(INS);
    if (*rb_p == 0x0f || *ra_p != 0x0f)
    {
        exception(ADR);
    }
    REGS[*rb_p] = *imm_p;
}

void OPC04_rmmovq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*func_p)
        exception(INS);
    if (*rb_p == 0x0f || *ra_p == 0x0f)
    {
        exception(ADR);
    }
    write_word_to_mem(REGS[*rb_p] + *imm_p, REGS[*ra_p]);
}

void OPC05_mrmovq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*func_p)
        exception(INS);
    if (*rb_p == 0x0f || *ra_p == 0x0f)
    {
        exception(ADR);
    }
    REGS[*rb_p] = read_word_from_mem(REGS[*ra_p] + *imm_p);
}

void OPC06_opq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    int64_t tmp;
    if (*rb_p == 0x0f || *ra_p == 0x0f)
    {
        exception(ADR);
    }
    switch (*func_p)
    {
    case 0:
        tmp = REGS[*ra_p] + REGS[*rb_p];
        // 两个加数符号位相同并且结果符号位与两者不同，则溢出
        if ((!((0x1 & REGS[*ra_p] >> 31) ^ (0x1 & REGS[*rb_p] >> 31))) &&
            ((0x1 & REGS[*ra_p] >> 31) ^ (0x1 & tmp >> 31)))
            OF = 0x1;
        else
            OF = 0x0;

        if (!tmp)
            ZF = 0x1;
        else
            ZF = 0x0;
        SF = 0x1 & tmp >> 31;

        REGS[*rb_p] = tmp;
        break;

    case 1:
        // 同加法, 第二个加数取反加一按加法处理
        // REGS[*rb_p] = ~REGS[*rb_p] + 1;
        tmp = REGS[*rb_p] - REGS[*ra_p];
        int64_t tra = REGS[*ra_p];

        if ((REGS[*ra_p] == ~REGS[*ra_p] + 1) && REGS[*rb_p] > 0)
            OF = 0x1;

        else
        {
            tra = ~tra + 1;
            if ((!((0x1 & REGS[*rb_p] >> 31) ^ (0x1 & tra >> 31))) &&
                ((0x1 & REGS[*rb_p] >> 31) ^ (0x1 & tmp >> 31)))
                OF = 0x1;
            else
                OF = 0x0;
        }

        if (!tmp)
            ZF = 0x1;
        else
            ZF = 0x0;
        SF = 0x1 & tmp >> 31;

        REGS[*rb_p] = tmp;
        break;

    case 2:
        // andq
        REGS[*rb_p] &= REGS[*ra_p];

        OF = 0;
        if (!REGS[*rb_p])
            ZF = 0x1;
        else
            ZF = 0;
        SF = 0x1 & tmp >> 31;
        break;

    case 3:
        // xorq
        REGS[*rb_p] ^= REGS[*ra_p];

        OF = 0;
        if (!REGS[*rb_p])
            ZF = 0x1;
        else
            ZF = 0;
        SF = 0x1 & tmp >> 31;
        break;

    default:
        exception(INS);
        break;
    }
}
void OPC07_jxx(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*imm_p < 0 || *imm_p >= MEM_LENGTH)
        exception(ADR);
    switch (*func_p)
    {
    case 0x00:
        // jmp
        PC = *imm_p;
        break;

    case 0x01:
        // jle
        if ((SF ^ OF | ZF) & 0x1)
            PC = *imm_p;
        break;

    case 0x02:
        // jl
        if ((SF ^ OF) & 0x1)
            PC = *imm_p;
        break;

    case 0x03:
        // je
        if (ZF & 0x1)
            PC = *imm_p;
        break;

    case 0x04:
        // jne
        {

            if ((~ZF) & 0x1)
                PC = *imm_p;
            break;
        }
    case 0x05:
        // jge
        if ((~(SF ^ OF)) & 0x1)
            PC = *imm_p;
        break;

    case 0x06:
        // jg
        if ((~(SF ^ OF) & ~ZF) & 0x1)
            PC = *imm_p;
        break;

    default:
        // others
        exception(INS);
        break;
    }
}
void OPC08_call(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*func_p)
        exception(INS);
    if (*imm_p < 0 || *imm_p >= MEM_LENGTH)
        exception(ADR);
    REGS[RSP] -= BYTES_PER_WORD;
    write_word_to_mem(REGS[RSP], PC);
    PC = *imm_p;
}
void OPC09_ret(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*func_p)
        exception(INS);
    int64_t tmp = read_word_from_mem(REGS[RSP]);
    if (tmp < 0 || tmp >= MEM_LENGTH)
        exception(ADR);
    REGS[RSP] += BYTES_PER_WORD;
    PC = tmp;
}
void OPC0A_pushq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*func_p)
        exception(INS);
    if (*ra_p == 0x0f || *rb_p != 0x0f)
    {
        exception(INS);
    }
    REGS[RSP] -= BYTES_PER_WORD;
    write_word_to_mem(REGS[RSP], REGS[*ra_p]);
}
void OPC0B_popq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p)
{
    if (*func_p)
        exception(INS);
    if (*ra_p == 0x0f || *rb_p != 0x0f)
    {
        exception(INS);
    }
    REGS[*ra_p] = read_word_from_mem(REGS[RSP]);
    REGS[RSP] += BYTES_PER_WORD;
}