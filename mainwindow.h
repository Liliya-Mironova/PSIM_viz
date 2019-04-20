#pragma once

#include <QMainWindow>
#include <QString>
#include "memory.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    int stage_num;
    uint32_t PC;
    int32_t PC_DISP;
    uint8_t check_J;
    uint8_t PC_R;
    int32_t BP_EX;
    uint32_t BP_MEM;
    uint32_t local_PC;
    uint8_t branch;

    Fetch_reg_new fetch_reg;
    Decode_reg decode_reg;
    Execute_reg execute_reg;
    Memory_reg memory_reg;
    Fetch_reg_new fetch_tmp;
    Decode_reg decode_tmp;
    Execute_reg execute_tmp;
    Memory_reg memory_tmp;
    HU HazardUnit;
    Insn_data_memory instr_data_mem;
    Regfile regfile;

public:
    MainWindow(QWidget *parent = 0, std::vector<uint32_t> insns = std::vector<uint32_t>(), uint32_t* regs = nullptr);
    void function();
    void setFields();
    void execute_one_stage(int stage_num);
    uint32_t get_PC();
    uint32_t get_PC_DISP();

    ~MainWindow();

private slots:
    void myclicked();

private:
    Ui::MainWindow *ui;
};
