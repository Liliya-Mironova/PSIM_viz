#include "mainwindow.h"
#include <QApplication>
#include <QString>
////PSIM////

////PSIM////

using namespace std;

int main(int argc, char *argv[])
{
    QString text = "fixed";
    QWidget *parent = 0;
    QApplication a(argc, argv);

    //std::vector<uint32_t> insns = {0b00000000010000011000000110110011, 0b01000000101000110000001010110011}; //add x3 , x3, x4

    std::vector<uint32_t> insns = { 0b00000000000000010010000010000011, // LW x1,x2,0
                                    0b00000000010100100000000110110011, // ADD x3,x4,x5
                                    0b01000000101000110000001010110011, // SUB x5,x6,x10
                                    0b00000000100100111000001101100011, // BEQ x7,x9,3
                                    0b00000000001000011000001100110011, // ADD x6,x3,x2
                                    0b01000000101000110000001010110011, // SUB x5,x6,x10
                                    0b00000000000000000000000001111111}; // STOP
    uint32_t regs[32] = {0, 2, 14000, 3, 1, 0, 10, 7, 3, 7, 4, 6, 2, 6, 2, 5, 3, 6, 2, 7, 6, 4, 2, 6, 9, 3, 3, 0, 8, 4, 5, 3};

    MainWindow w(parent, insns, regs);
    std::cout << w.get_PC() << std::endl;
    std::cout << w.get_PC_DISP() << std::endl;
    w.show();

    return a.exec();
}
