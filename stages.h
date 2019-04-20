#pragma once

#include <cassert>
#include "memory.h"

//на вход - память инструкций и регистров
//сигналы
//регистр после стадии и результат АЛУ

extern Fetch_reg_new fetch_stage(Insn_data_memory &mem, uint32_t &PC, int32_t &PC_DISP, uint8_t &PC_R, uint32_t &local_PC, uint8_t branch, uint8_t& check_J);

Decode_reg decode_stage(Fetch_reg_new &reg, Regfile &regfile, HU& HazardUnit);

Execute_reg execute_stage(Decode_reg &reg, int32_t &PC_DISP, uint8_t &PC_R, int32_t &BP_EX, uint32_t &BP_MEM, uint32_t &local_PC, uint8_t branch, HU& HazardUnit, uint8_t& check_J);

Memory_reg memory_stage(Execute_reg &reg, Insn_data_memory &mem, int32_t &BP_EX, uint8_t branch, HU& HazardUnit);

void write_back_stage(Memory_reg &reg, Regfile &regfile, uint32_t &BP_MEM, uint8_t &branch, HU& HazardUnit);

//#endif // STAGES_H
