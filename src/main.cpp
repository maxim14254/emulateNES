#include "mainwindow.h"
#include <QApplication>
#include "cpu.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    CPU cpu;
    bool rez = cpu.init_new_cartridge(":/games/Super Mario Bros. (World).nes");

    return a.exec();
}
