#pragma once

#include <bitset>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

struct Imm {
    uint32_t imm_I;
    uint32_t imm_S;
    uint32_t imm_B;
    uint32_t imm_U;
    uint32_t imm_J;
};

int32_t multiplexor5(int32_t rs2_val, Imm imm, uint32_t rd, uint8_t mux_ex); // зачем RD ???
int8_t multiplexor(int8_t lorD);
int32_t get_bits(uint32_t insn, unsigned int pos, unsigned int n);
int32_t sign_extend(uint32_t insn);
int32_t alu(uint ALUOp, int32_t a, int32_t b);

struct CU_signals {
    uint8_t WB_WE = 0;
    uint8_t MEM_WE = 0;
    uint8_t mux_ex = 0;
    uint8_t AluOp = 0;
    uint8_t mux_mem = 0;
    uint8_t conditional = 0;
    uint8_t stop = 0;

    uint8_t mux_pc = 0;
    uint8_t check_J = 0;

    CU_signals() {}

    void set_signals(uint8_t WB_WE_, uint8_t MEM_WE_, uint8_t mux_ex_,
                     uint8_t AluOp_, uint8_t mux_mem_, uint8_t conditional_, uint8_t stop_, uint8_t mux_pc_, uint8_t check_J_);

    void print_CU_signals();
};

struct CU_signals control_unit(uint32_t insn);
//-------------------------------------------------------------------------------------------------------
// TEMPORARY REGISTERS:

class Fetch_reg_new {
    uint32_t reg;
    uint32_t local_PC;

public:
    Fetch_reg_new() : reg(0), local_PC(0) {}
    Fetch_reg_new(uint32_t reg_, uint32_t local_PC_) { reg = reg_, local_PC = local_PC_; }

    uint32_t get_reg() { return reg; }
    uint32_t get_local_PC() { return local_PC; }

    void print_reg();
    void print_local_PC();
};

class Decode_reg {
    CU_signals CU_reg;
    uint8_t rs1;
    uint8_t rs2;
    int32_t rs1_val;
    int32_t rs2_val;
    int16_t imm1; //from 20 to 30
    int32_t imm2; //31
    uint8_t rd;

    uint32_t local_PC;
    uint8_t funct3;

public:
    Decode_reg() : rs1(0), rs2(0), rs1_val(0), rs2_val(0), imm1(0), imm2(0), rd(0), local_PC(0), funct3(0) {}
    Decode_reg(CU_signals CU_reg_, uint8_t rs1_, uint8_t rs2_, int32_t rs1_val_,
               int32_t rs2_val_, uint8_t rd_, int16_t imm1_, int32_t imm2_, uint32_t local_PC_, uint8_t funct3);

    CU_signals get_CU_reg() { return CU_reg; }
    uint8_t get_rs1() { return rs1; }
    uint8_t get_rs2() { return rs2; }
    uint32_t get_rs1_val() { return rs1_val; }
    uint32_t get_rs2_val() { return rs2_val; }
    uint8_t get_rd() { return rd; }
    uint16_t get_imm1() { return imm1; }
    uint32_t get_imm2() { return imm2; }
    uint32_t get_local_PC() { return local_PC; }
    uint8_t get_funct3() { return funct3; }
    void SetRs1Val(int32_t rs1_val_) { rs1_val = rs1_val_; }
    void SetRs2Val(int32_t rs2_val_) { rs2_val = rs2_val_; }
    void print_reg();
};

class Execute_reg {
    CU_signals CU_reg;
    int32_t rs2_val;
    int32_t ALUresult;
    uint8_t rd;
    bool flush = false;

public:
    Execute_reg() : rs2_val(0), ALUresult(0), rd(0) {std::cout << "EXECUTE_REG CONSTRUCTOR" ;}
    Execute_reg(CU_signals CU_, int32_t rs2_val, int32_t ALUresult_, uint8_t rd_, bool flush_);

    CU_signals get_CU_reg() { return CU_reg; }
    int32_t get_rs2_val() { return rs2_val; }
    int32_t get_ALUresult() { return ALUresult; }
    uint8_t get_rd() { return rd; }
    bool GetFlush() const { return flush; }

    void print_reg();
};

class Memory_reg {
    CU_signals CU_reg;
    uint32_t mux_result;
    uint8_t rd;

public:
    Memory_reg() : mux_result(0), rd(0) {std::cout << "MEMORY_REG CONSTRUCTOR" ;}
    Memory_reg(CU_signals CU_reg, uint32_t mux_result, uint8_t rd);

    CU_signals get_CU_reg() { return CU_reg; }
    uint32_t get_mux_res() { return mux_result; }
    uint8_t get_rd() { return rd; }

    void print_reg();
};

class HU {
    uint8_t rs1,
            rs2,
            rd_ex,
            rd_mem,
            rd_wb;
    uint32_t wb_res;
//	bool flush_again = false;
    public:
        void SetRs1(uint8_t rs1_) { rs1 = rs1_; }
        void SetRs2(uint8_t rs2_) { rs2 = rs2_; }
        void SetRdEx(uint8_t rd_ex_) { rd_ex = rd_ex_; }
        void SetRdMem(uint8_t rd_mem_) { rd_mem = rd_mem_; }
        void SetRdWb(uint8_t rd_wb_) { rd_wb = rd_wb_; }
        void SetWbRes(uint8_t wb_res_) { wb_res = wb_res_; }
        void Run(Fetch_reg_new& fetch, Decode_reg& decode, Execute_reg& execute, Memory_reg& mem);
};


//-------------------------------------------------------------------------------------------------------
// MEMORY:

//класс instn_Memory - хранит массив из 32 битных инструкций
class Insn_data_memory {
    std::vector<uint32_t> insn;

public:
    Insn_data_memory() {insn.reserve(256000);}

    void set_insn(std::vector<uint32_t> insns)
    { int j=0;

       // std::cout<<"set_nsn"<<std::endl;
      insn = insns;
      /*for(int i=0; i< ((65536/4) + insns.size()); i++)
      {
        //std::cout<<i<<std::endl;
        insn[i] = insns[j];
        //std::cout<<insns[j]<<std::endl;
        j++;
      }*/
    }

    uint32_t get_insn(int PC) { return insn[PC]; }
    void set_register(int32_t reg, uint32_t A) { assert(reg >= 0); insn[A] = reg; }
    uint32_t get_register(uint32_t reg) { assert(reg >= 0); return insn[reg]; }

    void print_memory();
};

class Regfile {
    uint32_t m_regs[32];

public:
    Regfile() {}
    Regfile(uint32_t regs[32]);
    void set_regfile(uint32_t regs[32]);

    uint32_t get_register(uint32_t number_register) { return m_regs[number_register]; }
    void set_register(uint8_t number_register, uint32_t word) { m_regs[number_register] = word; }

    void print_regfile();
};
