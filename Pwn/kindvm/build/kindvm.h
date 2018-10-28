#ifndef KINDVM_H
#define KINDVM_H

#include <stdint.h>

#define MAX_NAME_LEN	10
#define MAX_REG_NUMBER	8
#define MAX_MEM_RANGE	1024
#define MAX_INSN_LENGTH	1024
#define MAX_INSN_NUMBER	10

struct kindvm_control{
	unsigned int pc;
	unsigned int isstop;
	char *username;
	char *banner_file_name;
	void (*greeting)(void);
	void (*farewell)(void);
};

struct kindvm_control *kc;
int32_t *reg;
uint8_t *mem;
uint8_t *insn;

void (*func_table[MAX_INSN_NUMBER])(void);

void kindvm_abort(void);

void kindvm_setup(void);

char *input_username(void);

void input_insn(void);

void exec_insn(void);

uint8_t load_insn_uint8_t(void);
uint16_t load_insn_uint16_t(void);
uint32_t load_insn_uint32_t(void);

uint8_t load_mem_uint8_t(int16_t addr);
uint16_t load_mem_uint16_t(int16_t addr);
uint32_t load_mem_uint32_t(int16_t addr);

void store_mem_uint32_t(int16_t addr,uint32_t value);

unsigned int get_pc(void);
unsigned int step(void);

void insn_nop(void);	// 00
void insn_load(void);	// 01
void insn_store(void);	// 02
void insn_mov(void);	// 03
void insn_add(void);	// 04
void insn_sub(void);	// 05
void insn_halt(void);	// 06
void insn_in(void);	// 07
void insn_out(void);	// 08
void insn_hint(void);	// 09

void hint2(void);
void hint3(void);

void func_greeting(void);
void func_farewell(void);

#endif /* KINDVM_H */
