#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stages.h"
#include "memory.h"

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
    //ui -> check_J_ -> setText(QString::number(check_J));
    connect(ui->my_button, SIGNAL(clicked()), this, SLOT(myclicked()));
}

void MainWindow::myclicked() {
    std::cout << "clicked()" << std::endl;
    std::cout << PC << std::endl;
    std::cout << PC_DISP << std::endl;
    //execute_one_stage((stage_num++)%6);

    Fetch_reg_new fetch_tmp = fetch_stage(instr_data_mem, PC, PC_DISP, PC_R, local_PC, branch, check_J);
    fetch_tmp.print_reg();
    bool ok;
    QString hexString = (std::to_string(fetch_tmp.get_reg())).c_str();
    ui -> insn -> setText(QString("%1").arg(hexString.toULongLong(&ok, 16), 32, 2, QChar('0')));
    //ui -> insn -> setText(QString::number(fetch_tmp.get_reg(), 2));

    Decode_reg decode_tmp = decode_stage(fetch_reg, regfile, HazardUnit);
    decode_tmp.print_reg();
    std::cout << "DECODE_REG:" << std::endl;
    ui -> RS1 -> setText(QString::number(decode_tmp.get_rs1(), 2));
    ui -> RS2 -> setText(QString::number(decode_tmp.get_rs2(), 2));
    ui -> RS1_val -> setText(QString::number(decode_tmp.get_rs1_val(), 2));
    ui -> RS2_val -> setText(QString::number(decode_tmp.get_rs2_val(),2));
    ui -> imm1 -> setText(QString::number(decode_tmp.get_imm1(), 2));
    ui -> imm2 -> setText(QString::number(decode_tmp.get_imm2(), 2));
    ui -> rd -> setText(QString::number(decode_tmp.get_rd(), 2));
    ui -> local_PC_ -> setText(QString::number(decode_tmp.get_local_PC(), 2));

    Execute_reg execute_tmp = execute_stage(decode_reg, PC_DISP, PC_R, BP_EX, BP_MEM, local_PC, branch, HazardUnit, check_J);
    execute_tmp.print_reg();
    ui -> AluOp -> setText(QString::number(execute_tmp.get_CU_reg().AluOp, 2));
    ui -> mux_ex -> setText(QString::number(execute_tmp.get_CU_reg().mux_ex, 2));
    ui -> ALU_resultl -> setText(QString::number(execute_tmp.get_ALUresult(), 2));
    ui -> conditionall -> setText(QString::number(execute_tmp.get_CU_reg().conditional, 2));
    ui -> stop -> setText(QString::number(execute_tmp.get_CU_reg().stop, 2));
    ui -> RS1_val_execute -> setText(QString::number(decode_reg.get_rs1_val(), 2));
    ui -> RS2_val_execute -> setText(QString::number(decode_reg.get_rs2_val(), 2));
    ui -> RS1_val_2 -> setText(QString::number(execute_tmp.get_rs2_val(), 2));
    ui -> RS2_val_2 -> setText(QString::number(decode_reg.get_rs1_val(), 2));
    ui -> PC_R_ -> setText(QString::number(PC_R));

    Memory_reg memory_tmp = memory_stage(execute_reg, instr_data_mem, BP_EX, branch, HazardUnit);
    memory_tmp.print_reg();
    ui -> mux_mem -> setText(QString::number(memory_tmp.get_CU_reg().mux_mem, 2));
    ui -> MEM_WE -> setText(QString::number(memory_tmp.get_CU_reg().MEM_WE, 2));
    ui -> mux_result -> setText(QString::number(memory_tmp.get_mux_res(), 2));
    ui -> ALU_result_memory -> setText(QString::number(execute_reg.get_ALUresult(), 2));
    ui -> RS1_val_memory -> setText(QString::number(execute_reg.get_rs2_val(), 2));
    ui -> RD -> setText(QString::number(memory_tmp.get_rd(), 2));

    write_back_stage(memory_reg, regfile, BP_MEM, branch, HazardUnit);
    ui -> WB_WE -> setText(QString::number(memory_tmp.get_CU_reg().WB_WE, 2));
    ui -> WB_D -> setText(QString::number(memory_tmp.get_rd(), 2));
    ui -> stop -> setText(QString::number(memory_tmp.get_CU_reg().stop, 2));

    HazardUnit.Run(fetch_tmp, decode_tmp, execute_tmp, memory_tmp);
    fetch_reg = execute_tmp.GetFlush() ? Fetch_reg_new() : fetch_tmp;
    decode_reg = execute_tmp.GetFlush() ? Decode_reg() : decode_tmp;
    execute_reg = execute_tmp;
    memory_reg = memory_tmp;

    ui -> PC_ -> setText(QString::number(PC));
    ui -> PC_DISP_ -> setText(QString::number(PC_DISP));
    ui -> branch_ -> setText(QString::number(branch));
    //ui -> check_J_ -> setText(QString::number(check_J));
}

MainWindow::~MainWindow()
{
    delete ui;
}


uint32_t MainWindow::get_PC() {
    return PC;
}

uint32_t MainWindow::get_PC_DISP() {
    return PC_DISP;
}

