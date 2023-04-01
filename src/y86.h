#include <inttypes.h>

#define AOK 1
#define HLT 2
#define ADR 3
#define INS 4

#define MEM_LENGTH 1024
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
uchar MEM[MEM_LENGTH];
int64_t PC;

char reasons[4][4] = {"AOK", "HLT", "ADR", "INS"};
char reg_names[16][4] = {
    "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "NIL"};

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
void print_mem(int);
void end();
int read_from_file(char *filename, uchar *dest);

int get_arguments(int64_t PC, uchar *op_p, uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);

void OPC00_halt(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC01_nop(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC02_cmovxx(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC03_irmovq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC04_rmmovq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC05_mrmovq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC06_opq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC07_jxx(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC08_call(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC09_ret(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC0A_pushq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
void OPC0B_popq(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);

typedef void (*instr_func_p)(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
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
    OPC0B_popq
};