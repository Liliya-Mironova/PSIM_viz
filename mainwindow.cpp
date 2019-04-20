#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stages.h"

#include <QString>
#include <bitset>

using namespace std;

MainWindow::MainWindow(QWidget *parent, std::vector<uint32_t> insns, uint32_t* regs) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    stage_num = -1;
    ui->setupUi(this);

    PC = 0;
    PC_DISP = 0;
    check_J = 0;
    PC_R = 0;
    BP_EX = 0;
    BP_MEM = 0;
    local_PC = 0;
    branch = 0;

    instr_data_mem.set_insn(insns);
    instr_data_mem.print_memory();
    regfile.set_regfile(regs);

    QPixmap pm("/home/katze/PSIM_/Visio-RISC_V_pipeline_2018-2019_page-0001.jpg");
    ui -> picture -> setPixmap(pm);
    ui -> picture -> setScaledContents(true);
    ui -> PC_ -> setText(QString::number(PC));
    ui -> PC_DISP_ -> setText(QString::number(PC_DISP));
    ui -> PC_R_ -> setText(QString::number(PC_R));
    ui -> local_PC_ -> setText(QString::number(local_PC));
    ui -> branch_ -> setText(QString::number(branch));
    ui -> check_J_ -> setText(QString::number(check_J));
    connect(ui->my_button, SIGNAL(clicked()), this, SLOT(myclicked()));
}

void MainWindow::myclicked() {
    std::cout << "clicked()" << std::endl;
    std::cout << PC << std::endl;
    std::cout << PC_DISP << std::endl;
    execute_one_stage((stage_num++)%6);
    ui -> PC_ -> setText(QString::number(PC));
    ui -> PC_DISP_ -> setText(QString::number(PC_DISP));
    ui -> PC_R_ -> setText(QString::number(PC_R));
    ui -> branch_ -> setText(QString::number(branch));
    ui -> check_J_ -> setText(QString::number(check_J));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::execute_one_stage(int stage_num) {
    switch (stage_num)
    {
    case 0:
    {
        //fetch_reg = fetch_stage(instr_data_mem, PC, PC_DISP, PC_R, local_PC, branch, check_J);
        //fetch_reg.print_reg();
        fetch_tmp = fetch_stage(instr_data_mem, PC, PC_DISP, PC_R, local_PC, branch, check_J);
        fetch_tmp.print_reg();
        ui -> i -> setText(QString::number(stage_num));
        break;
    }
    case 1:
    {
        //decode_reg = decode_stage(fetch_reg, regfile, HazardUnit);
        //decode_reg.print_reg();
        decode_tmp = decode_stage(fetch_reg, regfile, HazardUnit);
        decode_tmp.print_reg();
        std::cout << "DECODE_REG:" << std::endl;
        ui -> RS1 -> setText(QString::number(decode_reg.get_rs1(), 2));
        ui -> RS2 -> setText(QString::number(decode_reg.get_rs2(), 2));
        ui -> RS1_val -> setText(QString::number(decode_reg.get_rs1_val(), 2));
        ui -> RS2_val -> setText(QString::number(decode_reg.get_rs2_val(),2));
        ui -> imm1 -> setText(QString::number(decode_reg.get_imm1(), 2));
        ui -> imm2 -> setText(QString::number(decode_reg.get_imm2(), 2));
        ui -> rd -> setText(QString::number(decode_reg.get_rd(), 2));
        ui -> local_PC_ -> setText(QString::number(decode_reg.get_local_PC(), 2));
        ui -> i -> setText(QString::number(stage_num));
        break;
    }
    case 2:
    {
        //execute_reg = execute_stage(decode_reg, PC_DISP, PC_R, BP_EX, BP_MEM, local_PC, branch, HazardUnit, check_J);
        execute_tmp = execute_stage(decode_reg, PC_DISP, PC_R, BP_EX, BP_MEM, local_PC, branch, HazardUnit, check_J);
        execute_tmp.print_reg();
        ui -> AluOp -> setText(QString::number(execute_reg.get_CU_reg().AluOp, 2));
        ui -> mux_ex -> setText(QString::number(execute_reg.get_CU_reg().mux_ex, 2));
        ui -> ALU_resultl -> setText(QString::number(execute_reg.get_ALUresult(), 2));
        ui -> conditionall -> setText(QString::number(execute_reg.get_CU_reg().conditional, 2));
        ui -> stop -> setText(QString::number(execute_reg.get_CU_reg().stop, 2));
        ui -> RS1_val_execute -> setText(QString::number(execute_reg.get_rs2_val(), 2));
        //execute_reg.print_reg();
        ui -> i -> setText(QString::number(stage_num));
        break;
    }
    case 3:
    {
        //memory_reg = memory_stage(execute_reg, instr_data_mem, BP_EX, branch, HazardUnit);
        memory_tmp = memory_stage(execute_reg, instr_data_mem, BP_EX, branch, HazardUnit);
        memory_tmp.print_reg();
        ui -> mux_mem -> setText(QString::number(memory_reg.get_CU_reg().mux_mem, 2));
        ui -> MEM_WE -> setText(QString::number(memory_reg.get_CU_reg().MEM_WE, 2));
        ui -> mux_result -> setText(QString::number(memory_reg.get_mux_res(), 2));
        ui -> RD -> setText(QString::number(memory_reg.get_rd(), 2));
        //memory_reg.print_reg();
        ui -> i -> setText(QString::number(stage_num));
        break;
    }
    case 4:
    {
        write_back_stage(memory_reg, regfile, BP_MEM, branch, HazardUnit);
        ui -> WB_WE -> setText(QString::number(memory_reg.get_CU_reg().WB_WE, 2));
        ui -> WB_D -> setText(QString::number(memory_reg.get_rd(), 2));
        ui -> i -> setText(QString::number(stage_num));
    }
    case 5:
    {
        fetch_reg = fetch_tmp;
        decode_reg = decode_tmp;
        execute_reg = execute_tmp;
        memory_reg = memory_tmp;
        ui -> i -> setText(QString::number(stage_num));
    }
    }
}

uint32_t MainWindow::get_PC() {
    return PC;
}

uint32_t MainWindow::get_PC_DISP() {
    return PC_DISP;
}

