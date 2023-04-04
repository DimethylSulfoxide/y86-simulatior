#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MEM_LENGTH 0x200
#define BYTES_PER_WORD 8
#define MAX_LENGTH_SINGLE_INSTR 30
typedef unsigned char uchar;
typedef void (*instr_func_p)(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
typedef int (*exec_single_instr_fp)(void);

int init(int, char *, int, char*);
uchar read_byte_from_mem(int64_t addr);
int write_byte_to_mem(int64_t addr, uchar byte);
int64_t read_word_from_mem(int64_t addr);
int write_word_to_mem(int64_t addr, int64_t word);
int exec_single_instr(void);
int exec_single_instr_debug(void);
int exception(int);
uchar get_ra(uchar rab);
uchar get_rb(uchar rab);
void print_regs();
void print_mem(int);
void end();
int read_from_file(char *filename, uchar *dest);
int write_to_mem(char *filename);
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
void OPC0C_list(uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);
int disasm(uchar *op_p, uchar *func_p, uchar *ra_p, uchar *rb_p, int64_t *imm_p);