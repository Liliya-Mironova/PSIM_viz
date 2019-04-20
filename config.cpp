#include "config.h"
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

std::vector<uint32_t> insns = {0b00000000010000011000000110110011}; //add x3 , x3, x4

//instr_data_mem.set_insn(insns);
//instr_data_mem.print_memory();

uint32_t regs[32] = {0, 2, 14000, 3, 1, 0, 10, 7, 3, 7, 4, 6, 2, 6, 2, 5, 3, 6, 2, 7, 6, 4, 2, 6, 9, 3, 3, 0, 8, 4, 5, 3};

Regfile regfile(regs);

void execute_stage(int stage_num) {
    switch (stage_num)
    {
    case 0:
    {
        Fetch_reg fetch_tmp = fetch(instr_data_mem, PC, PC_DISP, PC_R, local_PC, branch, check_J);
        fetch_tmp.print_reg();
        break;
    }
    case 1:
    {
        Decode_reg decode_tmp = decode(fetch_reg, regfile, HazardUnit);
        decode_tmp.print_reg();
        break;
    }
    case 2:
    {
        Execute_reg execute_tmp = execute(decode_reg, PC_DISP, PC_R, BP_EX, BP_MEM, local_PC, branch, HazardUnit, check_J);
        execute_tmp.print_reg();
        break;
    }
    case 3:
    {
        Memory_reg memory_tmp = memory(execute_reg, instr_data_mem, BP_EX, branch, HazardUnit);
        memory_tmp.print_reg();
        break;
    }
    case 4:
    {
        write_back(memory_reg, regfile, BP_MEM, branch, HazardUnit);
    }
    }
}
