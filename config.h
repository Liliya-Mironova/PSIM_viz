#ifndef CONFIG_H
#define CONFIG_H

#include "memory.h"
#include "stages.h"
#include <bitset>
#include <vector>

uint32_t PC;
int32_t PC_DISP;
uint8_t check_J;
uint8_t PC_R;
int32_t BP_EX;
uint32_t BP_MEM;
uint32_t local_PC;
uint8_t branch;
Fetch_reg fetch_reg;
Decode_reg decode_reg;
Execute_reg execute_reg;
Memory_reg memory_reg;
HU HazardUnit;


Insn_data_memory instr_data_mem;
//instr_data_mem.set_insn(insns);
//instr_data_mem.print_memory();

extern void execute_stage(int stage_num);

#endif // CONFIG_H
