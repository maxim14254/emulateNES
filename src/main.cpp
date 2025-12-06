#include "mainwindow.h"
#include <QApplication>
#include "cpu.h"
#include <QSurfaceFormat>
#include "log.h"
#include <QMetaObject>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

#ifdef LOG_ON
    LOG::Init();
#endif
//    MyOpenGL ll;
//    ll.resize(800, 600);
//    ll.show();

    CPU cpu(&w);
    bool rez = cpu.init_new_cartridge(":/games/nestest.nes");

    return a.exec();
}
