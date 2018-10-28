#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "kindvm.h"
#include "util.h"


void kindvm_abort(void){
	printf("Error! Try again!\n");
	exit(-1);
}

void kindvm_setup(void){
	// initialize kindvm_control
	kc = (struct kindvm_control *)malloc(sizeof(struct kindvm_control));
	kc->pc = 0x0;
	kc->isstop = 0x0;
	kc->username = input_username();
	kc->banner_file_name = "banner.txt";
	kc->greeting = func_greeting;
	kc->farewell = func_farewell;

	// initialize memory
	mem = (uint8_t *)malloc(sizeof(uint8_t) * MAX_MEM_RANGE);
	memset(mem,0x0,sizeof(uint8_t) * MAX_MEM_RANGE);

	// initialize register
	reg = (int32_t *)malloc(sizeof(int32_t) * MAX_REG_NUMBER);
	memset(reg,0x0,sizeof(int32_t) * MAX_REG_NUMBER);

	// initialize instructions
	insn = (uint8_t *)malloc(sizeof(uint8_t) * MAX_INSN_LENGTH);
	memset(mem,0x41,sizeof(uint8_t) * MAX_INSN_LENGTH);

	func_table[0] = insn_nop;
	func_table[1] = insn_load;
	func_table[2] = insn_store;
	func_table[3] = insn_mov;
	func_table[4] = insn_add;
	func_table[5] = insn_sub;
	func_table[6] = insn_halt;
	func_table[7] = insn_in;
	func_table[8] = insn_out;
	func_table[9] = insn_hint;

}

char *input_username(void){
	char buf[MAX_NAME_LEN];
	char *malloc_buf;
	unsigned int name_len;

	printf("Input your name : ");
	// For Hint Vulnerability
	gets(buf);

	malloc_buf = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
	name_len = strlen(buf);

	malloc_buf[MAX_NAME_LEN - 1] = '\0';
	strncpy(malloc_buf,buf,name_len);

	return malloc_buf;
}


void input_insn(void){
	printf("Input instruction : ");
	read(0,insn,MAX_INSN_LENGTH);
	
}

void exec_insn(void){
	uint8_t cur_insn;

	cur_insn = load_insn_uint8_t();
	if(!(0 <= cur_insn && cur_insn < MAX_INSN_NUMBER)){
		kindvm_abort();
	}

	func_table[cur_insn]();
}



uint8_t load_insn_uint8_t(void){
	return (uint8_t)insn[step()];
}

uint16_t load_insn_uint16_t(void){
	uint16_t value;

	value = (uint16_t)insn[get_pc()];
	step();
	value = value << 8;
	value = value + (uint16_t)insn[get_pc()];
	step();
	return value;
}

uint32_t load_insn_uint32_t(void){
	uint32_t value;

	value = (uint32_t)insn[get_pc()];
	step();
	value = value << 8;
	value = value + (uint32_t)insn[get_pc()];
	step();
	value = value << 8;
	value = value + (uint32_t)insn[get_pc()];
	step();
	value = value << 8;
	value = value + (uint32_t)insn[get_pc()];
	step();
	return value;
}

uint8_t load_mem_uint8_t(int16_t addr){
	return (uint8_t)mem[addr];
}

uint16_t load_mem_uint16_t(int16_t addr){
	uint16_t value;

	value = (uint16_t)mem[addr];
	value = value << 8;
	value = value + (uint16_t)mem[addr + 1];
	return value;
}

uint32_t load_mem_uint32_t(int16_t addr){
	uint32_t value;

	value = (uint32_t)mem[addr + 3];
	value = value << 8;
	value = value + (uint32_t)mem[addr + 2];
	value = value << 8;
	value = value + (uint32_t)mem[addr + 1];
	value = value << 8;
	value = value + (uint32_t)mem[addr];
	return value;
}


void store_mem_uint32_t(int16_t addr, uint32_t value){
	mem[addr + 3] = (value >> 24) & 255;
	mem[addr + 2] = (value >> 16) & 255;
	mem[addr + 1] = (value >> 8) & 255;
	mem[addr + 0] = value & 255;
}


unsigned int get_pc(void){
	return kc->pc;
}

unsigned int step(void){
	if(kc->pc >= MAX_INSN_LENGTH){
		kindvm_abort();
	}
	return kc->pc++;
}

void insn_nop(void){
	printf("NOP!\n");
}


void insn_load(void){
	uint8_t number;
	int16_t addr;

	number = load_insn_uint8_t();
	addr = (int16_t)load_insn_uint16_t();
	
	if(!(0 <= number && number < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	if(addr > MAX_MEM_RANGE - 4){
		kindvm_abort();
	}

	reg[number] = load_mem_uint32_t(addr);
}

void insn_store(void){
	int16_t addr;
	uint8_t number;

	addr = (int16_t)load_insn_uint16_t();
	number = load_insn_uint8_t();

	if(!(0 <= number && number < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	if(addr > MAX_MEM_RANGE - 4){
		kindvm_abort();
	}

	store_mem_uint32_t(addr,reg[number]);
}

void insn_mov(void){
	uint8_t dst,src;

	dst = load_insn_uint8_t();
	src = load_insn_uint8_t();

	if(!(0 <= dst && dst < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	if(!(0 <= src && src < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	reg[dst] = reg[src];
}

void insn_add(void){
	uint8_t dst,src;
	unsigned int hint;

	dst = load_insn_uint8_t();
	src = load_insn_uint8_t();

	if(!(0 <= dst && dst < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	if(!(0 <= src && src < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	if(reg[dst] >= 0){
		hint = 1;
	}

	reg[dst] = reg[dst] + reg[src];

	if(hint && reg[dst] < 0){
		hint3();
	}
}

void insn_sub(void){
	uint8_t dst,src;
	dst = load_insn_uint8_t();
	src = load_insn_uint8_t();

	if(!(0 <= dst && dst < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	if(!(0 <= src && src < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	reg[dst] = reg[dst] - reg[src];
}

void insn_halt(void){
	kc->isstop = 0x1;
}

void insn_in(void){
	uint8_t number;
	uint32_t value;

	number = load_insn_uint8_t();
	value = load_insn_uint32_t();

	if(!(0 <= number && number < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	reg[number] = value;

}

void insn_out(void){
	uint8_t number;

	number = load_insn_uint8_t();
	
	if(!(0 <= number && number < MAX_REG_NUMBER)){
		kindvm_abort();
	}

	printf("[out] %x(%d)\n",reg[number],reg[number]);
}

void insn_hint(void){
	hint2();
}

void hint2(void){
	open_read_write("hint2.txt");
	exit(-1);
}

void hint3(void){
	open_read_write("hint3.txt");
	exit(-1);
}
	

void func_greeting(void){
	open_read_write(kc->banner_file_name);
	write(1,"Instruction start!\n",20);
}

void func_farewell(void){
	open_read_write(kc->banner_file_name);
	write(1,"Execution is end! Thank you!\n",29);
}
