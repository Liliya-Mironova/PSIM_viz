#include "memory.h"

int32_t multiplexor5(int32_t rs2_val, Imm imm, uint32_t rd, uint8_t mux_ex) {
    switch (mux_ex) {
        case 0: return rs2_val;
        case 1: return imm.imm_I;
        case 2: return imm.imm_S;
        case 3: return imm.imm_B;
        case 4: return rd;
        case 5: return imm.imm_U;
        case 6: return imm.imm_J;
    }
}

int8_t multiplexor(int8_t lorD) {
    if (!lorD) return 0;
    return 1;
}

int32_t get_bits(uint32_t insn, unsigned int pos, unsigned int n) {
    return ((insn & (~(~0 << n) << pos)) >> pos);
}

int32_t sign_extend(uint32_t insn) {
    if (get_bits(insn, 31, 1)) return 0xFFFFFF<<11;
    return 0;
}

int32_t alu(uint ALUOp, int32_t a, int32_t b) {
    switch(ALUOp) {
        case 0: return a + b;
        case 1: return a - b;
        case 2: return a * b;
        case 3: return a / b;
        case 4: return b;
        case 5: return a & b;
        case 6: return a ^ b;
        case 7: return a | b;
        case 8: return a < b;
        case 9: return a << b;
        case 10: return a >> b;
        case 11: return a + 1;
    }
}

void CU_signals::set_signals(uint8_t WB_WE_, uint8_t MEM_WE_, uint8_t mux_ex_,
                             uint8_t AluOp_, uint8_t mux_mem_, uint8_t conditional_, uint8_t stop_, uint8_t mux_pc_, uint8_t check_J_) {
    WB_WE = WB_WE_;
    MEM_WE = MEM_WE_;
    mux_ex = mux_ex_;
    AluOp = AluOp_;
    mux_mem = mux_mem_;
    conditional = conditional_;
    stop = stop_;
    mux_pc = mux_pc_;
    check_J = check_J_;
}

void CU_signals::print_CU_signals() {
    std::cout << "CU_signals:" << std::endl;
    std::cout << "  WB_WE:       " << std::bitset<1>(WB_WE) << std::endl;
    std::cout << "  MEM_WE:      " << std::bitset<1>(MEM_WE) << std::endl;
    std::cout << "  AluOp:       " << std::bitset<2>(AluOp) << std::endl;
    std::cout << "  mux_ex:      " << std::bitset<2>(mux_ex) << std::endl;
    std::cout << "  mux_mem:     " << std::bitset<1>(mux_mem) << std::endl;
    std::cout << "  conditional: " << std::bitset<1>(conditional) << std::endl;
    std::cout << "  stop:        " << std::bitset<1>(stop) << std::endl;
    std::cout << "  mux_pc:      " << std::bitset<1>(mux_pc) << std::endl;
    std::cout << "  check_J:     " << std::bitset<1>(check_J) << std::endl;
}

CU_signals control_unit(uint32_t insn) {
    uint8_t opcode = get_bits(insn, 0, 7);
    uint8_t funct3 = get_bits(insn, 12, 3);
    uint8_t funct7 = get_bits(insn, 25, 7);

    CU_signals signals;
    //std::vector<int> signals_arr;

    //std::ofstream fout("log.txt");

//WB_WE, MEM_WE, mux_ex, AluOp, mux_mem, conditional, stop, mux_pc, check_J
    // R-type: mux_ex = 1 (rs1_val)
    if (opcode == 0b0110011) { // R-type, (ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND)
        switch (funct3) {
            case 0b000 : if (funct7 == 0b0000000) {
                            signals.set_signals(1,0,0,0,1,0,0,0,0); 		  // ADD rd,rs1,rs2
                            //signals_arr.insert(signals_arr.end(), {1,0,0,0,1,0,0,0,0})
                            std::cout << "ADD" << std::endl << std::endl;
                         } else if (funct7 == 0b0100000) {
                            signals.set_signals(1,0,0,1,1,0,0,0,0);  		  // SUB rd,rs1,rs2
                            std::cout << "SUB" << std::endl << std::endl;
                         }
                         break;
            case 0b001 : signals.set_signals(1,0,0,9,1,0,0,0,0); 			  // SLL rd,rs1,rs2	     rd ← ux(rs1) « rs2
                         std::cout << "SLL" << std::endl << std::endl;
                         break;
            case 0b010 : signals.set_signals(1,0,0,8,1,0,0,0,0); 			  // SLT rd,rs1,rs2	     rd ← sx(rs1) < sx(rs2)
                         std::cout << "SLT" << std::endl << std::endl;
                         break;
            case 0b011 : signals.set_signals(1,0,0,8,1,0,0,0,0);			  // SLTU rd,rs1,rs2     rd ← ux(rs1) < ux(rs2)
                         std::cout << "SLTU" << std::endl << std::endl;
                         break;
            case 0b100 : signals.set_signals(1,0,0,6,1,0,0,0,0);
                         std::cout << "XOR" << std::endl << std::endl;
                         break;
            case 0b101 : if (funct7 == 0b0000000) {
                            signals.set_signals(1,0,0,10,1,0,0,0,0);
                            std::cout << "SRL" << std::endl << std::endl; // SRL rd,rs1,rs2	     rd ← ux(rs1) » rs2
                         } else if (funct7 == 0b0100000) {
                            signals.set_signals(1,0,0,10,1,0,0,0,0);
                            std::cout << "SRA" << std::endl << std::endl; // SRA rd,rs1,rs2	     rd ← sx(rs1) » rs2
                         }
                         break;
            case 0b110 : signals.set_signals(1,0,0,7,1,0,0,0,0);
                         std::cout << "OR" << std::endl << std::endl; 	  // OR rd,rs1,rs2       rd ← ux(rs1) ∨ ux(rs2)
                         break;
            case 0b111 : signals.set_signals(1,0,0,5,1,0,0,0,0);
                         std::cout << "AND" << std::endl << std::endl;	  // AND rd,rs1,rs2      rd ← ux(rs1) ∧ ux(rs2)
                         break;
        }
    }
    // ( WB_WE_ 0, MEM_WE_ 0, mux_ex_ 5, AluOp_ 0, mux_mem_ 1, conditional_ 0, stop_ 0, pc 0)

    // I-type: mux_ex = 1 (imm_I)
/*	else if (opcode == 0b1100111) { // I-type (JALR)
        signals.set_signals(1,0,0,7,1,0,0,0); 							  // JALR rd,rs1,offset	rd ← pc + length(inst)
                                                                          // 			pc ← (rs1 + offset) ∧ -2
        std::cout << "JALR" << std::endl << std::endl;
    }*/
    else if (opcode == 0b0000011) { // I-type (LB, LH, LW, LBU, LHU)
        switch (funct3) { 	// WB_WE = 1 (write to register), mux_mem = 0 (write from memory)
            case 0b000: signals.set_signals(1,0,1,0,0,0,0,0,0); 			  // LB rd,offset(rs1)	 rd ← s8[rs1 + offset]
                        //fout << "LW x" << (int)get_bits(insn, 7, 5) << ", x" << (int)get_bits(insn, 15, 5) << ", " << (int)get_bits(insn, 20, 12) << "\n";
                        std::cout << "LB" << std::endl << std::endl;
                        break;
            case 0b001: signals.set_signals(1,0,1,0,0,0,0,0,0); 			  // LH rd,offset(rs1)	 rd ← s16[rs1 + offset]
                        std::cout << "LH" << std::endl << std::endl;
                        break;
            case 0b010: signals.set_signals(1,0,1,0,0,0,0,0,0); 			  // LW rd,offset(rs1)	 rd ← s32[rs1 + offset]
                        std::cout << "LW" << std::endl << std::endl;
                        break;
            case 0b100: signals.set_signals(1,0,1,0,0,0,0,0,0); 			  // LBU rd,offset(rs1)	 rd ← u8[rs1 + offset]
                        std::cout << "LBU" << std::endl << std::endl;
                        break;
            case 0b101: signals.set_signals(1,0,1,0,0,0,0,0,0); 			  // LHU rd,offset(rs1)	 rd ← u16[rs1 + offset]
                        std::cout << "LHU" << std::endl << std::endl;
                        break;
        }
    }
    else if (opcode == 0b0010011) { // I-type (ADDI, SLTI, SLTIU, XORI, ORI, ANDI)
        switch (funct3) { 	// WB_WE = 1 (write to register), mux_mem = 1 (write from ALU)
            case 0b000: signals.set_signals(1,0,1,0,1,0,0,0,0);
                        std::cout << "ADDI" << std::endl << std::endl;
                        break;
            case 0b010: signals.set_signals(1,0,1,8,1,0,0,0,0);           // SLTI rd,rs1,imm     rd ← sx(rs1) < sx(imm)
                        std::cout << "SLTI" << std::endl << std::endl;
                        break;
            case 0b011: signals.set_signals(1,0,1,8,1,0,0,0,0); 			// SLTIU rd,rs1,imm    rd ← sx(rs1) < sx(imm)
                        std::cout << "SLTIU" << std::endl << std::endl;
                        break;
            case 0b100: signals.set_signals(1,0,1,6,1,0,0,0,0); 			// XORI rd,rs1,imm     rd ← ux(rs1) ⊕ ux(imm)
                        std::cout << "XORI" << std::endl << std::endl;
                        break;
            case 0b110: signals.set_signals(1,0,1,7,1,0,0,0,0); 			// ORI rd,rs1,imm      rd ← ux(rs1) ∨ ux(imm)
                        std::cout << "ORI" << std::endl << std::endl;
                        break;
            case 0b111: signals.set_signals(1,0,1,5,1,0,0,0,0); 			// ANDI rd,rs1,imm     rd ← ux(rs1) ∧ ux(imm)
                        std::cout << "ANDI" << std::endl << std::endl;
                        break;
        }
    }
    // S-type: MEM_WE = 1, mux_ex = 2 (imm_S)
    else if (opcode == 0b0100011) { // S-type (SB, SH, SW)
        switch (funct3) {
            case 0b000:	signals.set_signals(0,1,2,0,0,0,0,0,0);			// SB rs2,offset(rs1)  u8[rs1 + offset] ← rs2
                        //fout << "SW x" << (int)get_bits(insn, 7, 5) << ", x" << (int)get_bits(insn, 15, 5) << ", " << (int)get_bits(insn, 20, 12) << "\n";
                        std::cout << "SB" << std::endl << std::endl;
                        break;
            case 0b001: signals.set_signals(0,1,2,0,0,0,0,0,0);			// SH rs2,offset(rs1)  u16[rs1 + offset] ← rs2
                        std::cout << "SH" << std::endl << std::endl;
                        break;
            case 0b010: signals.set_signals(0,1,2,0,0,0,0,0,0);			// SW rs2,offset(rs1)  u32[rs1 + offset] ← rs2
                        std::cout << "SW" << std::endl << std::endl;
                        break;
        }
    }
    // B-type: conditional = 1, mux_ex = 3 (imm_B)
    else if (opcode == 0b1100011) { // B-type (BEQ, BNE, BLT, BGE, BLTU, BGEU)
        switch (funct3) {
            case 0b000: signals.set_signals(0,0,3,0,0,1,0,0,0); 			// BEQ rs1,rs2,offset	if rs1 = rs2 then pc ← pc + offset
                        //signals_arr.insert(signals_arr.end(), {0,0,3,0,0,1,0,0,0})
                        std::cout << "BEQ" << std::endl << std::endl;
                        break;
            case 0b001: signals.set_signals(0,0,3,0,0,1,0,0,0); 			// BNE rs1,rs2,offset	if rs1 ≠ rs2 then pc ← pc + offset
                        std::cout << "BNE" << std::endl << std::endl;
                        break;
            case 0b100: signals.set_signals(0,0,3,0,0,1,0,0,0); 			// BLT rs1,rs2,offset	if rs1 < rs2 then pc ← pc + offset
                        std::cout << "BLT" << std::endl << std::endl;
                        break;
            case 0b101: signals.set_signals(0,0,3,0,0,1,0,0,0); 			// BGE rs1,rs2,offset	if rs1 ≥ rs2 then pc ← pc + offset
                        std::cout << "BGE" << std::endl << std::endl;
                        break;
            case 0b110: signals.set_signals(0,0,3,0,0,1,0,0,0); 			// BLTU rs1,rs2,offset	if rs1 < rs2 then pc ← pc + offset
                        std::cout << "BLTU" << std::endl << std::endl;
                        break;
            case 0b111: signals.set_signals(0,0,3,0,0,1,0,0,0); 			// BGEU rs1,rs2,offset	if rs1 ≥ rs2 then pc ← pc + offset
                        std::cout << "BGEU" << std::endl << std::endl;
                        break;
        }
    }
    // U-type: // WB_WE = 1 (write to registers), mux_ex = 5 (imm_U), ALuOp = 4 (do nothing)
    else if (opcode == 0b0110111) { 									// LUI rd,imm    	 	rd ← imm
        signals.set_signals(1,0,5,4,1,0,0,0,0);
        std::cout << "LUI" << std::endl << std::endl;
    }
    else if (opcode == 0b0010111) { 									// AUIPC rd,offset      rd ← pc + offset
        signals.set_signals(1,0,5,0,1,0,0,1,0); // mux_pc = 1 (add to pc), ALuOp = 0 (add)
        std::cout << "AUIPC" << std::endl << std::endl;
    }
    // J-type
    //  WB_WE_ 0, MEM_WE_ 0, mux_ex_ 5, AluOp_ 0, mux_mem_ 1, conditional_ 0, stop_ 0, pc 0
    else if (opcode == 0b1101111) {
        signals.set_signals(1,0,6,11,1,1,0,1,1);
        std::cout << "JAL" << std::endl << std::endl;					// JAL rd,offset     rd ← pc + length(inst)
                                                                        // 			 pc ← pc + offset
    }
    else if (opcode == 0b1111111) { 									// STOP
        signals.set_signals(0,0,0,0,0,0,1,0,0);
        std::cout << "STOP" << std::endl << std::endl;
    }

    //fout.close();
    return signals;
}
//---------------------------------------------------------------------------------------------------------------

void Fetch_reg_new::print_reg() {
    std::cout << "FETCH_REG:     0b" << std::bitset<32>(reg) << std::endl << std::endl;
}


void Fetch_reg_new::print_local_PC() {
    std::cout << "local PC:      0b" << std::bitset<32>(local_PC) << std::endl << std::endl;
}

Decode_reg::Decode_reg(CU_signals CU_reg_, uint8_t rs1_, uint8_t rs2_, int32_t rs1_val_, int32_t rs2_val_,
                        uint8_t rd_, int16_t imm1_, int32_t imm2_, uint32_t local_PC_, uint8_t funct3_) {
    CU_reg = CU_reg_;
    rs1 = rs1_;
    rs2 = rs2_;
    rs1_val = rs1_val_;
    rs2_val = rs2_val_;
    imm1 = imm1_;
    imm2 = imm2_;
    rd = rd_;
    local_PC = local_PC_;
    funct3 = funct3_;
}

void Decode_reg::print_reg() {
    std::cout << "DECODE_REG:" << std::endl;
    std::cout << "RS1:           0b" << std::bitset<8>(rs1) << std::endl;
    std::cout << "RS2:           0b" << std::bitset<8>(rs2) << std::endl;
    std::cout << "RS1_val:       0b" << std::bitset<32>(rs1_val) <<" "<< rs1_val << std::endl;
    std::cout << "RS2_val:       0b" << std::bitset<32>(rs2_val) <<" "<< rs2_val <<  std::endl;
    std::cout << "imm1:          0b" << std::bitset<16>(imm1) << std::endl;
    std::cout << "imm2:          0b" << std::bitset<32>(imm2) << std::endl;
    std::cout << "RD:            0b" << std::bitset<8>(rd) << std::endl << std::endl;
    std::cout << "local_PC:      0b" << std::bitset<8>(local_PC) << std::endl << std::endl;
}

Execute_reg::Execute_reg(CU_signals CU_reg_, int32_t rs2_val_, int32_t ALUresult_, uint8_t rd_, bool flush_) {
    CU_reg = CU_reg_;
    rs2_val = rs2_val_;
    ALUresult = ALUresult_;
    rd = rd_;
    flush = flush_;
}

void Execute_reg::print_reg() {
    std::cout << "EXECUTE_REG: " << std::endl;
    CU_reg.print_CU_signals();
    std::cout << "RS2_val:       0b" << std::bitset<32>(rs2_val) <<" "<<rs2_val << std::endl;
    std::cout << "ALUresult:     0b" << std::bitset<32>(ALUresult) <<" "<< ALUresult << std::endl;
    std::cout << "RD:            0b" << std::bitset<8>(rd) << std::endl << std::endl;
    std::cout << "FLUSH:           " << flush<< std::endl << std::endl;

}

Memory_reg::Memory_reg(CU_signals CU_reg_,uint32_t mux_result_,uint8_t rd_) {
    CU_reg = CU_reg_;
    mux_result = mux_result_;
    rd = rd_;
}

void Memory_reg::print_reg() {
    std::cout << "MEMORY_REG: " << std::endl;
    CU_reg.print_CU_signals();
    std::cout << "mux_result:    0b" << std::bitset<32>(mux_result) << std::endl;
    std::cout << "RD:            0b" << std::bitset<8>(rd) << std::endl << std::endl;
}


void HU::Run(Fetch_reg_new& fetch, Decode_reg& decode, Execute_reg& execute, Memory_reg& mem)
        {

            bool rs1_collision = false,
                 rs2_collision = false;

            if ((rs1 == rd_ex && rs1 != 0 && (rs1_collision = true)) || (rs2 == rd_ex && rs2 != 0 && (rs2_collision = true)))
            {
                //rs1_collision ? decode.SetRs1Val(execute.get_ALUresult()) : ;
                if(rs1_collision)
                    decode.SetRs1Val(execute.get_ALUresult());
                //rs2_collision ? decode.SetRs2Val(execute.get_ALUresult()) : ;
                if(rs2_collision)
                    decode.SetRs2Val(execute.get_ALUresult());
                //flush_again = true;
                //decode = Decode_reg();
                //fetch = Fetch_reg();
                return;
            }

            if ((rs1 == rd_mem  && rs1 != 0 && (rs1_collision = true)) || (rs2 == rd_mem && rs2 != 0 && (rs2_collision == true)))
            {
                //rs1_collision ? decode.SetRs1Val(mem.get_mux_res()) : ;
                if(rs1_collision)
                    decode.SetRs1Val(mem.get_mux_res());
                //rs2_collision ? decode.SetRs2Val(mem.get_mux_res()) : ;
                if(rs2_collision)
                    decode.SetRs2Val(mem.get_mux_res());
                //decode = Decode_reg();
                //fetch = Fetch_reg();
                return;
            }

            if ((rs1 == rd_wb  && rs1 != 0 && (rs1_collision == true)) || (rs2 == rd_wb && rs2 != 0 && (rs2_collision == true)))
            {
                //rs1_collision ? decode.SetRs1Val(wb_res) : ;
                if(rs1_collision)
                    decode.SetRs1Val(wb_res);
                //rs2_collision ? decode.SetRs2Val(wb_res) : ;
                if(rs2_collision)
                    decode.SetRs2Val(wb_res);
                fetch = Fetch_reg_new();
                return;
            }
        }
//---------------------------------------------------------------------------------------------------------------

void Insn_data_memory::print_memory() {
    std::cout << "INSN_DATA_MEMORY:" << std::endl;
    for (uint32_t PC = 0; PC < insn.size(); PC++)
        std::cout << "0b" << std::bitset<32>(insn[PC]) << std::endl;
    std::cout << std::endl;
}

Regfile::Regfile(uint32_t regs[32]) {
    m_regs[0] = 0;
    for (int i = 1; i < 32; i++)
        m_regs[i] = regs[i];
}

void Regfile::set_regfile(uint32_t regs[32]) {
    m_regs[0] = 0;
    for (int i = 1; i < 32; i++)
        m_regs[i] = regs[i];
}

void Regfile::print_regfile() {
    std::cout << "REGFILE:" << std::endl;
    for (int i = 0; i < 32; ++i)
        std::cout << "x" << i << ": 0b" << std::bitset<32>(m_regs[i]) << std::endl;
    std::cout << std::endl;
}
