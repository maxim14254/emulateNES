#include "mainwindow.h"
#include <QApplication>
#include "cpu.h"
#include <QSurfaceFormat>
#include <QMetaObject>
#include "bus.h"
#include "ppu.h"

#ifdef LOG_ON
#include "log.h"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setAttribute(Qt::AA_ForceRasterWidgets, false);

    MainWindow w;
    w.show();

    Bus bus;

    PPU ppu(&w, &bus);
    bus.init_PPU(&ppu);

#ifdef LOG_ON
    LOG::Init(&ppu);
#endif

    CPU cpu(&w, &bus);
    bus.init_CPU(&cpu);
    bool rez = cpu.slot_init_new_cartridge(":/games/nestest.nes");

    return a.exec();
}
