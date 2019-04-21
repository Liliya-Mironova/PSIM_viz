#include "stages.h"

Fetch_reg_new fetch_stage(Insn_data_memory &mem, uint32_t &PC, int32_t &PC_DISP, uint8_t &PC_R, uint32_t &local_PC, uint8_t branch, uint8_t& check_J) {

    Fetch_reg_new return_reg;
    if (PC_R)
    {
        std::cout<<"LOCAL_PC = "<<local_PC<<std::endl;
        std::cout<<"PC_DISP = "<<PC_DISP<<std::endl;
        std::cout<<"check_J = "<<std::bitset<8>(check_J)<<std::endl;
        PC = check_J ? (PC_DISP/4)  : local_PC + static_cast<int32_t>(PC_DISP) ;
        std::cout<<"PC = "<<PC<<std::endl<<std::endl;

        assert(PC >= 0);
        branch = 1;
        return_reg = Fetch_reg_new(mem.get_insn(PC), PC); // read instruction from memory
        PC+=1;
    }

    else
    {
        std::cout<<"PC: "<<PC<<std::endl<<std::endl;
        return_reg = Fetch_reg_new(mem.get_insn(PC), PC); // read instruction from memory
        PC += 1;
        branch = 0;
    }


    return return_reg;
}

Decode_reg decode_stage(Fetch_reg_new &reg, Regfile &regfile, HU& HazardUnit) {
    CU_signals CU = control_unit(reg.get_reg()); //create Control Unit

    uint32_t local_PC = reg.get_local_PC();
    std::cout<<"reg.get_reg: "<<std::bitset<32>(reg.get_reg())<<std::endl;
    uint8_t	rs1 = get_bits(reg.get_reg(), 15, 5); // get data
    uint8_t	rs2 = get_bits(reg.get_reg(), 20, 5);
    uint8_t	rd = get_bits(reg.get_reg(), 7, 5);
    int16_t imm1 = get_bits(reg.get_reg(), 20, 11);
    int32_t imm2 = get_bits(reg.get_reg(), 31, 1);
    uint8_t funct3 = get_bits(reg.get_reg(), 12, 3);

    uint32_t sgn = sign_extend(reg.get_reg()); // sign extend immediate
    uint32_t rs1_val = regfile.get_register(rs1); // read data from regfile
    uint32_t rs2_val = regfile.get_register(rs2);

    HazardUnit.SetRs1(rs1);
    HazardUnit.SetRs2(rs2);
    //take here not imm2 , but sign_extended(imm2)
    Decode_reg return_reg(CU, rs1, rs2, rs1_val, rs2_val, rd, imm1, sgn, local_PC, funct3);
    return return_reg;
}

Execute_reg execute_stage(Decode_reg &reg, int32_t &PC_DISP, uint8_t &PC_R, int32_t &BP_EX, uint32_t &BP_MEM, uint32_t &local_PC, uint8_t branch, HU& HazardUnit, uint8_t& check_J) {
    CU_signals CU = reg.get_CU_reg(); // get signals

//Signals//
    uint8_t AluOp = CU.AluOp;
    uint8_t mux_ex = CU.mux_ex;
    uint8_t conditional = CU.conditional;
    uint8_t WB_WE = CU.WB_WE;
    uint8_t mux_pc = CU.mux_pc;
    check_J = CU.check_J;
    std::cout<<"check_J = "<<std::bitset<8>(check_J)<<std::endl;
//Signals//

    local_PC = reg.get_local_PC();
    int32_t rs1_val = reg.get_rs1_val(); // get data // !!!!!!!!!!!!!!! 32
    int32_t rs2_val = reg.get_rs2_val(); // !!!!!!!!!!!!!!!!!!!!!!!!!!! 32
    int16_t imm1 = reg.get_imm1(); //from 20 to 30 bit
    int32_t imm2 = reg.get_imm2(); // 31 bit is here sign extended!!( uint32_t )
    uint8_t rd = reg.get_rd(); // from 7 to 11 bit

    uint8_t rs1 = reg.get_rs1();
    uint8_t funct3 = reg.get_funct3();


    //std::cout<<"imm1 is: "<<std::bitset<32>(imm1)<<std::endl;
    //std::cout<<"imm2 is: "<<std::bitset<32>(imm2)<<std::endl;

    struct Imm imm;
    imm.imm_I = imm1 | imm2;
    imm.imm_S = imm2 | ((get_bits(imm1,5,6) << 5 ) | rd);
    imm.imm_B = imm2 | ((get_bits(rd,1,4) | get_bits(imm1,5,5) << 4 | get_bits(imm1,10,1) << 9 | get_bits(imm2,31,1) << 10 ));
    imm.imm_U = imm1 << 8 | rs1 << 3 | funct3;
    imm.imm_J = funct3 | rs1 << 3 | get_bits(imm1,10,1) << 8 | get_bits(imm1,0,9) << 9 | get_bits(imm2,31,1) << 19;
    //std::cout<<"immidiate B is: "<<std::bitset<32>(imm.imm_B)<<std::endl;

    // extra multiplexor and signal for choosing between rs1_val or PC !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (mux_pc)
        rs1 = local_PC;

    int32_t mux5_res = multiplexor5(rs2_val, imm, rd, mux_ex); // mux5
    PC_DISP = mux5_res;

    std::cout<<std::endl<<"PC_DISP: "<<std::bitset<32>(PC_DISP)<<" = "<<static_cast<int32_t>(PC_DISP)<<std::endl;
    std::cout<<"TO check brunch:"<<std::endl;
    std::cout<<"RS1_val: "<<rs1_val<<std::endl;
    std::cout<<"RS2_val: "<<rs2_val<<std::endl<<std::endl;
    //PC_DISP just return imm.imm_B
    int32_t ALUresult = alu(AluOp, rs1_val, mux5_res); // ALU
    bool flush = false;

    if (conditional == 1 && rs1_val == rs2_val) { // comparator - conditional
        PC_R = 1;
        branch = 1;
        flush = true;
        std::cout << "it is a branch" << std::endl;
    } else if (check_J == 1) {
        PC_R = 1;
        branch = 1;
        flush = true;
        std::cout << "it is a JAL" << std::endl;
    }
    else {
        PC_R = 0;
    }

    if (WB_WE)
    {
        HazardUnit.SetRdEx(rd);
    }
    else
    {
        HazardUnit.SetRdEx(0);
    }

    Execute_reg return_reg(CU, rs2_val, ALUresult, rd, flush);
    return return_reg;
}

Memory_reg memory_stage(Execute_reg &reg, Insn_data_memory &mem, int32_t &BP_EX, uint8_t branch, HU& HazardUnit) {
    std::cout << "MEMORY!!" << std::endl;
    CU_signals CU = reg.get_CU_reg(); // get signals
    uint8_t mux_mem = CU.mux_mem;
    uint8_t MEM_WE = CU.MEM_WE;
    uint8_t WB_WE = CU.WB_WE;
    //std::cout << "after signals detecting" << std::endl;

    int32_t rs2_val = reg.get_rs2_val(); // get data
    int32_t get_alu_result = reg.get_ALUresult();

    uint32_t rd;
    //std::cout << "before get_register" << std::endl;
    if (!MEM_WE && get_alu_result >= 0)
        rd = mem.get_register(get_alu_result); // read from memory anyway
    //std::cout << "ad" << std::endl;
    std::cout<<rs2_val<<std::endl<<get_alu_result<<std::endl;
    if (MEM_WE & (!branch) )
    {
        assert(get_alu_result >= 0);
        mem.set_register(rs2_val, get_alu_result); // write to memory only on signal
    }
    //std::cout << "before mux_mem" << std::endl;
    int32_t result_d;
    if (!mux_mem) // multiplexor
        result_d = rd;
    else
        result_d = get_alu_result;
    //std::cout << "after mux_mem" << std::endl;

    BP_EX = reg.get_ALUresult(); // set signal
    //std::cout << "ALU" << std::endl;
    if (WB_WE)
    {
        HazardUnit.SetRdMem(reg.get_rd());
    }
    else
    {
        HazardUnit.SetRdMem(0);
    }
    //std::cout << "just return" << std::endl;
    Memory_reg return_reg(CU, result_d, reg.get_rd());
    return return_reg;
}

void write_back_stage(Memory_reg &reg, Regfile &regfile, uint32_t &BP_MEM, uint8_t &branch, HU& HazardUnit) {
    CU_signals CU = reg.get_CU_reg(); // get signals
    uint8_t WB_WE_signal = CU.WB_WE;
    uint8_t STOP_signal = CU.stop;

    if (STOP_signal == 1) {
        std::cout << "STOP PIPELINE!" << std::endl << std::endl;
        exit(-1);
    }

    uint32_t WB_D = reg.get_mux_res(); // set signals
    BP_MEM = reg.get_mux_res();
    uint8_t WB_A = reg.get_rd();

    if (WB_WE_signal)
    {
        HazardUnit.SetRdWb(reg.get_rd());
        HazardUnit.SetWbRes(WB_D);
    }
    else
    {
        HazardUnit.SetRdWb(0);
    }

    if (WB_WE_signal & (!branch)){
        regfile.set_register(WB_A, WB_D); // write to regfile only on signal
        std::cout << "[reg]: " << std::bitset<8>(WB_A) << " -> " << std::bitset<32>(regfile.get_register(WB_A)) << std::endl;
    }
}
